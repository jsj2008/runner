#include <jni.h>
#include <stdio.h>
#include <android/log.h>
#include <android/asset_manager_jni.h>
#include <runner.h>

JNIEXPORT jint JNICALL Java_ua_org_asqz_runner_Wrapper_init (JNIEnv* env, jclass wrapper, jobject assetManager)
{
   AAssetManager* manager = AAssetManager_fromJava(env, assetManager);
   return init(manager);
}

JNIEXPORT jint JNICALL Java_ua_org_asqz_runner_Wrapper_restore (JNIEnv* env, jclass wrapper)
{
   return restore();
}

JNIEXPORT int JNICALL Java_ua_org_asqz_runner_Wrapper_update (JNIEnv* env, jclass wrapper)
{
   return update();
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_shutdown (JNIEnv* env, jclass wrapper)
{
   shutdown();
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_resize (JNIEnv* env, jclass wrapper, jint width, jint height)
{
   resize(width, height);
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_activated (JNIEnv* env, jclass wrapper)
{
   activated();
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_deactivated (JNIEnv* env, jclass wrapper)
{
   deactivated();
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

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_key_1down (JNIEnv* env, jclass wrapper, jint key)
{
   key_down(key);
}

JNIEXPORT void JNICALL Java_ua_org_asqz_runner_Wrapper_key_1up (JNIEnv* env, jclass wrapper, jint key)
{
   key_up(key);
}

