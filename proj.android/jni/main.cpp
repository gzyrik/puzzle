#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include <jni.h>
#include <android/log.h>
#include "AppDelegate.h"

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  CLASS_NAME "org/cocos2dx/cpp/AppActivity"

using namespace cocos2d;

void cocos_android_app_init (JNIEnv* env) {
    LOGD("cocos_android_app_init");
    AppDelegate *pAppDelegate = new AppDelegate();
}

static std::function<void(cocos2d::Texture2D*)> _completion;
void pickImage(const std::function<void()>& transition,
               const std::function<void(cocos2d::Texture2D*)>& completion)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "pickImage", "()V")) {
        _completion = completion;
        t.env->CallStaticObjectMethod(t.classID, t.methodID);
        t.env->DeleteLocalRef(t.classID);
    }
    else {
        completion(0);
    }
}

extern "C" {
    JNIEXPORT void JNICALL Java_org_cocos2dx_cpp_AppActivity_nativeOnPickImageCallback(JNIEnv* env, jobject thiz, jstring imagePathh)
    {
        cocos2d::Texture2D* tex2D = 0;
        const std::string& ret = JniHelper::jstring2string(imagePathh);
        LOGD("pickImage:%s", ret.c_str());
        if (ret.size() > 0)
            tex2D = cocos2d::Director::getInstance()->getTextureCache()->addImage(ret);
        _completion(tex2D);
    }
}
