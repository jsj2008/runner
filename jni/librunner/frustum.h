#pragma once

#include "mathlib.h"

struct cam_t;
struct bbox_t;

typedef struct frustum_t
{
   plane_t planes[6];
} frustum_t;

void frustum_set(frustum_t* frustum, const struct cam_t* camera);
void frustum_show(const frustum_t* frustum);
int frustum_intersect_aabb(const frustum_t* frustum, const struct bbox_t* aabb);

