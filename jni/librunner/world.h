#pragma once

#include "mathlib.h"
#include "material.h"
#include "camera.h"
#include "bbox.h"

typedef struct vertex_t
{
   vec3f_t point;
   vec3f_t normal;
   vec2f_t uv;
} vertex_t;

typedef struct submesh_t
{
   char material[64];

   unsigned long nindices;
   unsigned long nvertices;

   unsigned int* indices;
   struct vertex_t* vertices;
} submesh_t;

typedef struct mesh_t
{
   char name[64];

   unsigned long nsubmeshes;
   struct submesh_t* submeshes;
} mesh_t;

typedef struct phys_t
{
   enum
   {
      PHYS_NOCOLLISION = 0,
      PHYS_RIGID,
      PHYS_STATIC,
   } type;

   float mass;
   float friction;
} phys_t;

typedef struct node_t
{
   char name[64];
   char data[64];

   enum
   {
      NODE_MESH = 0,
      NODE_CAMERA,
      NODE_LAMP,
   } type;

   mat4f_t transform;
   bbox_t bbox;

   struct phys_t phys;

   long parent_index;
} node_t;

typedef struct texture_t
{
   char name[64];
   char path[64];

   unsigned long min_filter;
   unsigned long mag_filter;
   unsigned long wrap_s;
   unsigned long wrap_t;
} texture_t;

typedef struct scene_t
{
   char name[64];
   char camera[64];

   vec3f_t gravity;

   unsigned long nnodes;
   struct node_t* nodes;
} scene_t;

typedef struct world_t
{
   char name[64];

   unsigned long ncameras;
   unsigned long nmaterials;
   unsigned long ntextures;
   unsigned long nmeshes;
   unsigned long nscenes;

   struct camera_t* cameras;
   struct material_t* materials;
   struct texture_t* textures;
   struct mesh_t* meshes;
   struct scene_t* scenes;
} world_t;

int world_load_from_file(world_t** pworld, const char* fname);
int world_init(world_t** pworld, char* data);
void world_free(world_t* world);

camera_t* world_get_camera(const world_t* world, const char* name);
material_t* world_get_material(const world_t* world, const char* name);
texture_t* world_get_texture(const world_t* world, const char* name);
mesh_t* world_get_mesh(const world_t* world, const char* name);
scene_t* world_get_scene(const world_t* world, const char* name);
node_t* scene_get_node(const scene_t* scene, const char* name);

void world_render_mesh(const world_t* world, const struct camera_t* camera, const mesh_t* mesh, const mat4f_t* transform);

