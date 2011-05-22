#pragma once

#include "mathlib.h"

struct cam_t;

typedef struct bbox_t
{
   vec3f_t min;
   vec3f_t max;
} bbox_t;

void bbox_reset(bbox_t* b);
void bbox_inflate(bbox_t* b, vec3f_t* v);
void bbox_show(const bbox_t* b);
int bbox_tri_intersection(const bbox_t* bbox, const vec3f_t* a, const vec3f_t* b, const vec3f_t* c);
void bbox_draw(const bbox_t* b, const struct cam_t* camera);

