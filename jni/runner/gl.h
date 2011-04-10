#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include "common.h"
#include "tex2d.h"

void outGLString(const char* name, GLenum e);
void checkGLError(const char* op);
GLuint createShaderProgram(const char* vertexShaderSrc, const char* pixelShaderSrc);

GLuint gl_load_texture(tex2d_t* t);
GLuint gl_load_checkered_texture();

