#pragma once

#include "vector.h"

#define MAX_TEXTURES 4

typedef struct vertex_t
{
   vec4f_t pos;
   vec4f_t color;
   float tex_coord[2]; 
} vertex_t;

typedef struct mesh_t
{
   int shader;
   mat4f_t transform;

   int nvertices;
   vertex_t* vertices;

   int ntextures;
   int tex_ids[MAX_TEXTURES];

} mesh_t;

void mesh_load_cube(mesh_t* m);
int mesh_load_from_obj(mesh_t* m, const char* fname);
void mesh_free(mesh_t* m);

