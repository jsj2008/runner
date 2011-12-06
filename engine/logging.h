#pragma once

#ifdef ANDROID
#include <android/log.h>
#define TAG "RUNNER"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);
#else
#include <stdio.h>
#define LOGI(...) { printf(__VA_ARGS__); putchar('\n'); }
#define LOGD(...) { /*printf(__VA_ARGS__); putchar('\n');*/ }
#define LOGE(...) { printf(__VA_ARGS__); putchar('\n'); }
#endif

