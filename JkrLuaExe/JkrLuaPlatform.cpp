#include "ksai_config.hpp"
#include "JkrLuaExe.hpp"

#ifdef ANDROID
#include <jni.h>
JavaVM* g_vm      = nullptr;
jobject g_context = nullptr;

extern "C" JNIEXPORT void JNICALL Java_org_JkrGUI_JkrGUIActivity_InitJNI(JNIEnv* env,
                                                                         jobject thiz) {
    ksai_print("INITIALIZED::JNI");
    env->GetJavaVM(&g_vm);
    g_context = env->NewGlobalRef(thiz);
}

void AndroidShowToast(const char* inMessage) {
    JNIEnv* env;
    if (g_vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        ksai_print("Failed to attach current thread");
        return;
    }

    jstring Message    = env->NewStringUTF(inMessage);
    jclass JkrGUIClass = env->FindClass("org/JkrGUI/JkrGUIActivity");
    jmethodID methodId = env->GetMethodID(JkrGUIClass, "ShowToast", "(Ljava/lang/String;)V");
    env->CallVoidMethod(g_context, methodId, Message);
}
#endif

void LuaShowToastNotification(const std::string_view inMessage) {
#ifdef ANDROID
    AndroidShowToast(inMessage.data());
#endif
}

namespace JkrEXE {
void CreatePlatformBindings(sol::state& inS) {
    auto Jkr = inS["Jkr"].get_or_create<sol::table>();
    Jkr.set_function("ShowToastNotification", LuaShowToastNotification);
}

}; // namespace JkrEXE