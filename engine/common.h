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
#define GL_COMPRESSED_RGB_S3TC_DXT1_EXT   0x83F0
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT  0x83F1
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT  0x83F2
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT  0x83F3
#define glDepthRange glDepthRangef
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glext.h>

#define GL_ETC1_RGB8_OES                     0x8D64
#define GL_ATC_RGB_AMD                       0x8C92
#define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD       0x8C93
#define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD   0x87EE
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG   0x8C00
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG   0x8C01
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG  0x8C02
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG  0x8C03

#endif

#ifdef ANDROID
#define TAG "RUNNER"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__);
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__);
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__);
#else
#define LOGI(...) { printf(__VA_ARGS__); putchar('\n'); }
#define LOGD(...) { /*printf(__VA_ARGS__); putchar('\n');*/ }
#define LOGE(...) { printf(__VA_ARGS__); putchar('\n'); }
#endif

void outGLString(const char* name, GLenum e);
void checkGLError(const char* op);
int isGLExtensionSupported(const char* name);

