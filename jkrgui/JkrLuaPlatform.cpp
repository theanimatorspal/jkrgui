#include "ksai_config.hpp"
#include "JkrLuaExe.hpp"
#ifdef __WIN32__
#include <Windows.h>
#include <commdlg.h.>
#endif

#ifdef ANDROID
#include <jni.h>
JavaVM *g_vm      = nullptr;
jobject g_context = nullptr;

extern "C" JNIEXPORT void JNICALL Java_org_JkrGUI_JkrGUIActivity_InitJNI(JNIEnv *env,
                                                                         jobject thiz) {
    ksai_print("INITIALIZED::JNI");
    env->GetJavaVM(&g_vm);
    g_context = env->NewGlobalRef(thiz);
}

extern "C" JNIEXPORT void JNICALL Java_org_JkrGUI_JkrGUIActivity_ChangeDirectory(
     JNIEnv *inenv, jobject thiz, jstring inDirectoryName) {

    jboolean isCopy;
    auto charrs = inenv->GetStringUTFChars(inDirectoryName, &isCopy);
    std::string directory(charrs);
    inenv->ReleaseStringUTFChars(inDirectoryName, charrs);
    std::filesystem::current_path(directory);
}

void AndroidShowToast(const char *inMessage) {
    std::string Message = inMessage;
    JNIEnv *env;
    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_6;
    args.name    = NULL;
    args.group   = NULL;
    if (g_vm->AttachCurrentThread(&env, &args) != JNI_OK) {
        ksai_print("Failed to attach current thread");
        return;
    }
    jstring jMessage   = env->NewStringUTF(Message.c_str());
    jclass JkrGUIClass = env->FindClass("org/JkrGUI/JkrGUIActivity");
    jmethodID methodId = env->GetMethodID(JkrGUIClass, "ShowToast", "(Ljava/lang/String;)V");
    env->CallVoidMethod(g_context, methodId, jMessage);
}

glm::vec3 AndroidGetAccelerometerData() {
    glm::vec3 outValue{};
    JNIEnv *env;
    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_6;
    args.name    = NULL;
    args.group   = NULL;
    if (g_vm->AttachCurrentThread(&env, &args) != JNI_OK) {
        ksai_print("Failed to attach current thread");
        return outValue;
    }
    jclass JkrGUIClass = env->FindClass("org/JkrGUI/JkrGUIActivity");
    jmethodID methodId = env->GetMethodID(JkrGUIClass, "GetAccelerometerData", "()[F");
    jfloatArray accelerometerDataArray = (jfloatArray)env->CallObjectMethod(g_context, methodId);
    jfloat *data                       = env->GetFloatArrayElements(accelerometerDataArray, NULL);
    jsize length                       = env->GetArrayLength(accelerometerDataArray);

    if (length >= 3) {
        outValue.x = data[0];
        outValue.y = data[1];
        outValue.z = data[2];
    } else {
        ksai_print("ERROR: Length of Accelerometer Data is <= 3");
    }

    env->ReleaseFloatArrayElements(accelerometerDataArray, data, 0);
    return outValue;
}

#endif

void LuaShowToastNotification(const std::string inMessage) {
#ifdef ANDROID
    AndroidShowToast(inMessage.c_str());
#else
    std::cout << inMessage.data();
#endif
}

std::string CreateFilterString(const std::vector<std::pair<std::string, std::string>> &filters) {
    std::string filterStr;
    for (const auto &filter : filters) {
        filterStr += filter.first + '\0';  // Description of the file type
        filterStr += filter.second + '\0'; // Corresponding filter (e.g., *.txt)
    }
    return filterStr + '\0'; // End with double null terminator
}

namespace JkrEXE {
void CreatePlatformBindings(sol::state &inS) {
    auto Jkr = inS["Jkr"].get_or_create<sol::table>();
    Jkr.set_function("ShowToastNotification", LuaShowToastNotification);
#ifdef ANDROID
    Jkr.set_function("GetAccelerometerData", AndroidGetAccelerometerData);
#endif

#ifdef __WIN32__
    Jkr.set_function(
         "GetFilePathByFilePicker",
         [](std::string inDescriptionOfFileType, std::string inFileTypeExtension) -> std::string {
             OPENFILENAME ofn;
             char szFile[512];

             // List of filters (description, extension)
             std::vector<std::pair<std::string, std::string>> filters = {
                  {inDescriptionOfFileType, inFileTypeExtension},
                  // {"Image Files", "*.jpg;*.png;*.bmp"},
                  {"All Files", "*.*"}};

             // Create the filter string
             std::string filterStr = CreateFilterString(filters);

             // Initialize the OPENFILENAME structure
             ZeroMemory(&ofn, sizeof(ofn));
             ofn.lStructSize    = sizeof(ofn);
             ofn.hwndOwner      = NULL;
             ofn.lpstrFile      = szFile;
             ofn.lpstrFile[0]   = '\0';
             ofn.nMaxFile       = sizeof(szFile);
             ofn.lpstrFilter    = filterStr.c_str(); // Assign the dynamically created filter string
             ofn.nFilterIndex   = 1;
             ofn.lpstrFileTitle = NULL;
             ofn.nMaxFileTitle  = 0;
             ofn.lpstrInitialDir = NULL;
             ofn.Flags           = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

             // Display the Open dialog box
             if (GetOpenFileName(&ofn) == TRUE) {
                 std::cout << "Selected file: " << ofn.lpstrFile << std::endl;
             } else {
                 std::cout << "File selection cancelled." << std::endl;
             }
             return std::string(szFile);
         });
#endif
}

}; // namespace JkrEXE