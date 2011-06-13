#pragma once

typedef struct shader_t shader_t;

int shader_load(shader_t** pshader, const char* name);
void shader_free(shader_t* shader);

void shader_use(const shader_t* shader);
void shader_unuse(const shader_t* shader);
void shader_set_attrib_vertices(shader_t* shader, const char* name, long components, long type, long stride, const void* values);
void shader_set_uniform_matrices(shader_t* shader, const char* name, long count, const float* values);
void shader_set_uniform_vectors(shader_t* shader, const char* name, long count, const float* values);
void shader_set_uniform_integers(shader_t* shader, const char* name, long count, const int* values);

