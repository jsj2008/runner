#pragma once

struct cam_t;
struct vertex_t;

typedef struct octree_t octree_t;

void octree_build(octree_t** po, struct vertex_t* vertices, long nvertices, int* indices, long nindices);
int octree_load(octree_t** po, const char* fname);
int octree_save(const octree_t* o, const char* fname);
void octree_free(octree_t* o);
void octree_show(const octree_t* o);
void octree_draw(const octree_t* o, const struct cam_t* camera);

