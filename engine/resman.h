#pragma once

struct world_t;
struct shader_t;
struct tex2d_t;
struct material_t;
struct mesh_t;

typedef struct resman_t resman_t;

int resman_init(resman_t** prm, const struct world_t* world);
void resman_free(resman_t* rm);
void resman_show(const resman_t* rm);

struct shader_t* resman_get_shader(resman_t* rm, const char* name);
struct tex2d_t* resman_get_texture(resman_t* rm, const char* name);
struct material_t* resman_get_material(resman_t* rm, const char* name);
struct mesh_t* resman_get_mesh(resman_t* rm, const char* name);

