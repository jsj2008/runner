#include "frustum.h"
#include "common.h"
#include "bbox.h"

void frustum_set(frustum_t* frustum, const mat4f_t* proj, const mat4f_t* view)
{
   mat4f_t clip;
   mat4_mult(&clip, proj, view);

   // right plane
   frustum->planes[0].x = clip.m41 - clip.m11;
   frustum->planes[0].y = clip.m42 - clip.m12;
   frustum->planes[0].z = clip.m43 - clip.m13;
   frustum->planes[0].w = clip.m44 - clip.m14;

   // left plane
   frustum->planes[1].x = clip.m41 + clip.m11;
   frustum->planes[1].y = clip.m42 + clip.m12;
   frustum->planes[1].z = clip.m43 + clip.m13;
   frustum->planes[1].w = clip.m44 + clip.m14;

   // top plane
   frustum->planes[2].x = clip.m41 - clip.m21;
   frustum->planes[2].y = clip.m42 - clip.m22;
   frustum->planes[2].z = clip.m43 - clip.m23;
   frustum->planes[2].w = clip.m44 - clip.m24;

   // bottom plane
   frustum->planes[3].x = clip.m41 + clip.m21;
   frustum->planes[3].y = clip.m42 + clip.m22;
   frustum->planes[3].z = clip.m43 + clip.m23;
   frustum->planes[3].w = clip.m44 + clip.m24;

   // far plane
   frustum->planes[4].x = clip.m41 - clip.m31;
   frustum->planes[4].y = clip.m42 - clip.m32;
   frustum->planes[4].z = clip.m43 - clip.m33;
   frustum->planes[4].w = clip.m44 - clip.m34;

   // near plane
   frustum->planes[5].x = clip.m41 + clip.m31;
   frustum->planes[5].y = clip.m42 + clip.m32;
   frustum->planes[5].z = clip.m43 + clip.m33;
   frustum->planes[5].w = clip.m44 + clip.m34;

   int i = 0;
   for (i = 0; i < 6; ++i)
   {
      plane_t* p = &frustum->planes[i];
      float len = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
      p->x /= len;
      p->y /= len;
      p->z /= len;
      p->w /= len;
   }
}

void frustum_show(const frustum_t* frustum)
{
   LOGI("Frustum:");
   int i = 0;
   for (i = 0; i < 6; ++i)
   {
      LOGI("\t%.2f %.2f %.2f %.2f", frustum->planes[i].x, frustum->planes[i].y, frustum->planes[i].z, frustum->planes[i].w);
   }
}
/*
int frustum_intersect_aabb(const frustum_t* frustum, const bbox_t* aabb)
{
   const vec3f_t* min = &aabb->min;
   const vec3f_t* max = &aabb->max;

   int i = 0;
   for (i = 0; i < 6; ++i)
   {
      const plane_t* plane = &frustum->planes[i];

      if (plane->x * min->x + plane->y * max->y + plane->z * min->z + plane->w > 0.0f)
         continue;

      if (plane->x * min->x + plane->y * max->y + plane->z * max->z + plane->w > 0.0f)
         continue;

      if (plane->x * max->x + plane->y * max->y + plane->z * max->z + plane->w > 0.0f)
         continue;

      if (plane->x * max->x + plane->y * max->y + plane->z * min->z + plane->w > 0.0f)
         continue;

      if (plane->x * max->x + plane->y * min->y + plane->z * min->z + plane->w > 0.0f)
         continue;

      if (plane->x * max->x + plane->y * min->y + plane->z * max->z + plane->w > 0.0f)
         continue;

      if (plane->x * min->x + plane->y * min->y + plane->z * max->z + plane->w > 0.0f)
         continue;

      if (plane->x * min->x + plane->y * min->y + plane->z * min->z + plane->w > 0.0f)
         continue;

      return -1;
   }
   return 0;
}
*/
int frustum_intersect_aabb(const frustum_t* frustum, const bbox_t* aabb)
{
   const vec3f_t* min = &aabb->min;
   const vec3f_t* max = &aabb->max;
   vec3f_t tmp;

   int result = 1;
   //  1 - inside
   //  0 - intersect
   // -1 - outside

   int i = 0;
   for (i = 0; i < 6; ++i)
   {
      const plane_t* plane = &frustum->planes[i];

      tmp.x = (plane->x < 0.0f) ? min->x : max->x;
      tmp.y = (plane->y < 0.0f) ? min->y : max->y;
      tmp.z = (plane->z < 0.0f) ? min->z : max->z;

      if (plane->x * tmp.x + plane->y * tmp.y + plane->z * tmp.z + plane->w < 0.0f)
         return -1;

      tmp.x = (plane->x < 0.0f) ? max->x : min->x;
      tmp.y = (plane->y < 0.0f) ? max->y : min->y;
      tmp.z = (plane->z < 0.0f) ? max->z : min->z;

      if (plane->x * tmp.x + plane->y * tmp.y + plane->z * tmp.z + plane->w <= 0.0f)
         result = 0;
   }

   return result;
}

