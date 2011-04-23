#pragma once

#include "bbox.h"
#include "model.h"
#include "camera.h"

typedef struct octree_node_t
{
   bbox_t bbox;
   long parent;
   long children[8];

   long ntris;
   int* tris;
} octree_node_t;

typedef struct octree_t
{
   long nvertices;
   long nindices;
   long nnodes;

   vertex_t* vertices;
   int* indices;
   octree_node_t* nodes;
} octree_t;

void octree_build(octree_t** po, vertex_t* vertices, long nvertices, int* indices, long nindices);
int octree_load(octree_t** po, const char* fname);
int octree_save(const octree_t* o, const char* fname);
void octree_free(octree_t* o);
void octree_show(const octree_t* o);
void octree_draw(const octree_t* o, const cam_t* camera);

