#include "ksai_config.hpp"
#include "JkrLuaExe.hpp"

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

///@warning remove this
void JavaCallVoidMethodNoArgs(sv inClass, sv inName) {
    JNIEnv *env;
    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_6;
    args.name    = NULL;
    args.group   = NULL;
    if (g_vm->AttachCurrentThread(&env, &args) != JNI_OK) {
        ksai_print("Failed to attach current thread");
        return;
    }
    jclass JkrGUIClass = env->FindClass("org/JkrGUI/JkrGUIActivity");
    jmethodID methodId = env->GetMethodID(JkrGUIClass, "ShowToast", "()V");
    env->CallVoidMethod(g_context, methodId);
}

///@warning remove this
void JavaCallVoidMethodWithStringArgs(std::string_view inClass,
                                      std::string_view inMethod,
                                      std::string_view argument) {
    JNIEnv *env;
    JavaVMAttachArgs args;
    args.version = JNI_VERSION_1_6;
    args.name    = nullptr;
    args.group   = nullptr;

    if (g_vm->AttachCurrentThread(&env, &args) != JNI_OK) {
        printf("Failed to attach current thread\n");
        return;
    }

    jclass javaClass = env->FindClass(inClass.data());
    if (javaClass == nullptr) {
        printf("Failed to find class: %s\n", inClass.data());
        return;
    }

    jmethodID methodId = env->GetMethodID(javaClass, inMethod.data(), "(Ljava/lang/String;)V");
    if (methodId == nullptr) {
        printf("Failed to find method: %s\n", inMethod.data());
        env->DeleteLocalRef(javaClass);
        return;
    }

    jstring jArg = env->NewStringUTF(argument.data());
    env->CallVoidMethod(g_context, methodId, jArg);
    env->DeleteLocalRef(jArg);
    env->DeleteLocalRef(javaClass);
}

std::vector<char> JavaCallVCharMethodStringArg(std::string_view className,
                                               std::string_view methodName,
                                               std::string_view argument) {
    std::vector<char> result;
    JNIEnv *env;
    if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        std::cerr << "Failed to attach current thread to JVM" << std::endl;
        return result;
    }
    jclass javaClass = env->FindClass(className.data());
    if (!javaClass) {
        std::cerr << "Failed to find Java class: " << className << std::endl;
        return result;
    }
    jmethodID methodId = env->GetMethodID(
         javaClass, methodName.data(), "(Ljava/lang/String;)[C"); // (String) -> char[]
    if (!methodId) {
        std::cerr << "Failed to find Java method: " << methodName << std::endl;
        env->DeleteLocalRef(javaClass);
        return result;
    }
    jstring jArg         = env->NewStringUTF(argument.data());
    jcharArray charArray = (jcharArray)env->CallObjectMethod(g_context, methodId, jArg);
    if (charArray) {
        jsize length = env->GetArrayLength(charArray);
        jchar *chars = env->GetCharArrayElements(charArray, nullptr);

        for (jsize i = 0; i < length; ++i) {
            result.push_back(static_cast<char>(chars[i])); // Convert jchar to char
        }
        env->ReleaseCharArrayElements(charArray, chars, JNI_ABORT);
        env->DeleteLocalRef(charArray);
    }
    env->DeleteLocalRef(jArg);
    env->DeleteLocalRef(javaClass);

    return result;
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
    Jkr.set_function("JavaCallVoidMethodNoArgs", &JavaCallVoidMethodNoArgs);
    Jkr.set_function("JavaCallVoidMethodWithStringArgs", &JavaCallVoidMethodWithStringArgs);
    Jkr.set_function("JavaCallVCharMethodStringArg", &JavaCallVCharMethodStringArg);
#endif
}

}; // namespace JkrEXE