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

extern "C" JNIEXPORT void JNICALL Java_org_JkrGUI_JkrGUIActivity_ChangeDirectory(
     JNIEnv *inenv, jobject thiz, jstring inDirectoryName) {

    jboolean isCopy;
    auto charrs = inenv->GetStringUTFChars(inDirectoryName, &isCopy);
    std::string directory(charrs);
    inenv->ReleaseStringUTFChars(inDirectoryName, charrs);
    std::filesystem::current_path(directory);
}
#endif

void LuaShowToastNotification(const std::string inMessage) {
#ifdef ANDROID
    AndroidShowToast(inMessage.c_str());
#else
    std::cout << inMessage.data();
#endif
}

namespace JkrEXE {
void CreatePlatformBindings(sol::state &inS) {
    auto Jkr = inS["Jkr"].get_or_create<sol::table>();
    Jkr.set_function("ShowToastNotification", LuaShowToastNotification);
}

}; // namespace JkrEXE