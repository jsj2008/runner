#pragma once

#include "vector.h"
#include "camera.h"
#include "bbox.h"

typedef struct frustum_t
{
   vec4f_t planes[6];
} frustum_t;

void frustum_set(frustum_t* frustum, const cam_t* camera);
void frustum_show(const frustum_t* frustum);
int frustum_intersect_aabb(const frustum_t* frustum, const bbox_t* aabb);

