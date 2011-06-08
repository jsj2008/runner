#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>
#include <GL/glut.h>
#endif

#ifdef ANDROID
#define TAG "RUNNER"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);
#else
#define LOGI(...) { printf(__VA_ARGS__); putchar('\n'); }
#define LOGE(...) { printf(__VA_ARGS__); putchar('\n'); }
#endif

void outGLString(const char* name, GLenum e);
void checkGLError(const char* op);

