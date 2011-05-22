#pragma once

#include "vector.h"
#include "matrix.h"
#include "camera.h"

#define MAX_TEXTURES_PER_MESH 4
#define MAX_BONES_PER_VERTEX 2

typedef struct vertex_t
{
   vec3f_t pos;
   vec3f_t normal;
   float tex_coord[2];

   float bone[MAX_BONES_PER_VERTEX];
   float weight[MAX_BONES_PER_VERTEX];
} vertex_t;

typedef struct mesh_t
{
   char name[64];
   char material[64];

   long nvertices;
   long nindices;

   vertex_t* vertices;
   int* indices;
} mesh_t;

typedef struct bone_t
{
   char name[32];
   int parent;
   vec3f_t pos;
   quat_t quat;
} bone_t;

typedef struct anim_t
{
   char name[32];

   float fps;

   long nframes;

   float* transforms;
} anim_t;

typedef struct model_t
{
   char name[64];

   long nbones;
   long nanims;
   long nmeshes;

   bone_t* bones;
   anim_t* anims;
   mesh_t* meshes;
} model_t;

int model_load(model_t** model, const char* fname);
int model_save(const model_t* model, const char* fname);
void model_free(const model_t* model);
void model_render(const model_t* model, const cam_t* camera, int frame, const mat4f_t* transform);

