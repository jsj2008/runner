#pragma once

#include "mathlib.h"
#include "material.h"
#include "camera.h"
#include "bbox.h"

typedef struct vertex_t
{
   vec3f_t point;
   vec3f_t normal;
} vertex_t;

typedef struct uvmap_t
{
   char name[64];

   unsigned long nuvs;

   vec2f_t* uvs;
} uvmap_t;

typedef struct submesh_t
{
   char material[64];

   unsigned long nindices;

   unsigned int* indices;
} submesh_t;

typedef struct mesh_t
{
   char name[64];

   unsigned long active_uvmap;

   unsigned long nvertices;
   unsigned long nuvmaps;
   unsigned long nsubmeshes;

   struct vertex_t* vertices;
   struct uvmap_t* uvmaps;
   struct submesh_t* submeshes;
} mesh_t;

typedef struct shape_t
{
   enum
   {
      SHAPE_BOX = 0,
      SHAPE_SPHERE,
      SHAPE_CAPSULE,
      SHAPE_CONE,
      SHAPE_CYLINDER,
      SHAPE_CONVEX,
      SHAPE_CONCAVE,
   } type;

   float margin;
   float radius;
   vec3f_t extents;
} shape_t;

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
   float restitution;

   float linear_damping;
   float angular_damping;

   float linear_sleeping_threshold;
   float angular_sleeping_threshold;

   float inertia_factor;

   vec3f_t linear_factor;
   vec3f_t angular_factor;

   struct shape_t shape;
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

typedef struct lamp_t
{
   char name[64];

   enum
   {
      LAMP_POINT = 0,
      LAMP_SPOT,
   } type;

   enum
   {
      FALLOFF_INV_LINEAR = 0,
      FALLOFF_INV_SQUARED,
      FALLOFF_CONST,
   } falloff_type;

   float energy;
   float distance;
   float spot_size;
   float spot_blend;

   color_t color;
} lamp_t;

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
   unsigned long nlamps;
   unsigned long nscenes;

   struct camera_t* cameras;
   struct material_t* materials;
   struct texture_t* textures;
   struct mesh_t* meshes;
   struct lamp_t* lamps;
   struct scene_t* scenes;
} world_t;

int world_load_from_file(world_t** pworld, const char* fname);
int world_init(world_t** pworld, char* data);
void world_free(world_t* world);

camera_t* world_get_camera(const world_t* world, const char* name);
material_t* world_get_material(const world_t* world, const char* name);
texture_t* world_get_texture(const world_t* world, const char* name);
mesh_t* world_get_mesh(const world_t* world, const char* name);
lamp_t* world_get_lamp(const world_t* world, const char* name);
scene_t* world_get_scene(const world_t* world, const char* name);
node_t* scene_get_node(const scene_t* scene, const char* name);
node_t* scene_pick_node(const world_t* world, const scene_t* scene, const vec2f_t* point);

void world_render_mesh(const world_t* world, const struct camera_t* camera, const mesh_t* mesh, const mat4f_t* transform);
void world_render_camera(const world_t* world, const camera_t* camera, const camera_t* cam, const mat4f_t* transform);
void world_render_lamp(const world_t* world, const camera_t* camera, const lamp_t* lamp, const mat4f_t* transform);

