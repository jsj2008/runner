#include <jni.h>
#include <android/log.h>
#include "runner.h"

JNIEXPORT jint JNICALL Java_ua_org_asqz_runner_Wrapper_init (JNIEnv* env, jclass wrapper, jstring apkPath)
{
   const char* str = (*env)->GetStringUTFChars(env, apkPath, 0);
   int res = init(str);
   (*env)->ReleaseStringUTFChars(env, apkPath, str);
   return res;
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_shutdown (JNIEnv* env, jclass wrapper)
{
   shutdown();
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_resize (JNIEnv* env, jclass wrapper, jint width, jint height)
{
   resize(width, height);
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_update (JNIEnv* env, jclass wrapper)
{
   update();
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_scroll (JNIEnv* env, jclass wrapper, jlong dt, jfloat dx1, jfloat dy1, jfloat dx2, jfloat dy2)
{
   scroll(dt, dx1, dy1, dx2, dy2);
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_pointer_1down (JNIEnv* env, jclass wrapper, jint pointerId, jfloat x, jfloat y)
{
   pointer_down(pointerId, x, y);
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_pointer_1up (JNIEnv* env, jclass wrapper, jint pointerId, jfloat x, jfloat y)
{
   pointer_up(pointerId, x, y);
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_pointer_1move (JNIEnv* env, jclass wrapper, jint pointerId, jfloat x, jfloat y)
{
   pointer_move(pointerId, x, y);
}

