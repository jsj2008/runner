#pragma once

#include "vector.h"

typedef struct bbox_t
{
   vec4f_t min;
   vec4f_t max;
} bbox_t;

void bbox_reset(bbox_t* b);
void bbox_inflate(bbox_t* b, vec4f_t* v);
void bbox_show(const bbox_t* b);
int bbox_tri_intersection(const bbox_t* bbox, const vec4f_t* a, const vec4f_t* b, const vec4f_t* c);

