#pragma once

#include "common.h"

struct shader_t;
struct tex2d_t;
struct model_t;
struct octree_t;

typedef struct resman_t resman_t;

int resman_init(resman_t** prm);
void resman_free(resman_t* rm);
void resman_show(const resman_t* rm);

struct shader_t* resman_get_shader(resman_t* rm, const char* name);
struct tex2d_t* resman_get_texture(resman_t* rm, const char* name);
struct model_t* resman_get_model(resman_t* rm, const char* name);
struct octree_t* resman_get_octree(resman_t* rm, const char* name);

