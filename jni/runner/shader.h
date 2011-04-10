#pragma once

#include "common.h"

#define MAX_SHADER_VARS 32

typedef struct shader_var_t
{
   char name[32];
   long location;
} shader_var_t;

typedef struct shader_t
{
   char name[64];
   long program;

   long nvars;
   shader_var_t vars[MAX_SHADER_VARS];
} shader_t;

int shader_load(shader_t* shader, const char* name);
void shader_free(shader_t* shader);

void shader_use(const shader_t* shader);
void shader_set_attrib_vertices(shader_t* shader, const char* name, long components, long type, long stride, const void* values);
void shader_set_uniform_matrices(shader_t* shader, const char* name, long count, const float* values);
void shader_set_uniform_integers(shader_t* shader, const char* name, long count, const int* values);

