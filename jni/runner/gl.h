#pragma once

#include "common.h"
#include "tex2d.h"

void outGLString(const char* name, GLenum e);
void checkGLError(const char* op);

GLuint gl_load_texture(tex2d_t* t);
GLuint gl_load_checkered_texture();

