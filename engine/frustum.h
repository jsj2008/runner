#pragma once

#include "mathlib.h"

struct bbox_t;

typedef struct frustum_t
{
   plane_t planes[6];
} frustum_t;

void frustum_set(frustum_t* frustum, const mat4f_t* proj, const mat4f_t* view);
void frustum_show(const frustum_t* frustum);
int frustum_intersect_aabb(const frustum_t* frustum, const struct bbox_t* aabb);

