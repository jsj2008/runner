#pragma once

#include "vector.h"
#include "matrix.h"
#include "camera.h"

#define MAX_TEXTURES 4
#define MAX_BONES_PER_VERTEX 2

typedef struct vertex_t
{
   vec4f_t pos;
   vec4f_t normal;
   vec4f_t color;
   float tex_coord[2];

   float bone[MAX_BONES_PER_VERTEX];
   float weight[MAX_BONES_PER_VERTEX];
} vertex_t;

typedef struct mesh_t
{
   char name[64];
   char shader[64];

   long ntextures;
   long nvertices;
   long nindices;

   char textures[MAX_TEXTURES][64];
   vertex_t* vertices;
   int* indices;
} mesh_t;

typedef struct bone_t
{
   char name[32];
   int parent;
   vec4f_t pos;
   vec4f_t quat;
} bone_t;

typedef struct anim_t
{
   char name[32];

   long nframes;

   float* transforms;
} anim_t;

typedef struct model_t
{
   char name[64];

   mat4f_t transform;

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
void model_render(const model_t* model, const cam_t* camera, int frame);

