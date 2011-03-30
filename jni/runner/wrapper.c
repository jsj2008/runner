#include <jni.h>
#include <android/log.h>
#include "runner.h"

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_init (JNIEnv* env, jclass wrapper, jstring apkPath)
{
   const char* str = (*env)->GetStringUTFChars(env, apkPath, 0);
   init(str);
   (*env)->ReleaseStringUTFChars(env, apkPath, str);
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_resize (JNIEnv* env, jclass wrapper, jint width, jint height)
{
   resize(width, height);
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_update (JNIEnv* env, jclass wrapper)
{
   update();
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_scroll (JNIEnv* env, jclass wrapper, jlong delta_time, jfloat delta_x, jfloat delta_y)
{
   scroll(delta_time, delta_x, delta_y);
}

