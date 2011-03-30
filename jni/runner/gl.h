#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "common.h"

void outGLString(const char* name, GLenum e);
void checkGLError(const char* op);
GLuint createShaderProgram(const char* vertexShaderSrc, const char* pixelShaderSrc);
GLuint createTexture(const char* fname, int* width, int* height);
GLuint createCheckeredTexture();

