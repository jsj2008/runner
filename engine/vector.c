#include "mathlib.h"

vec3f_t* vec3_normalize(vec3f_t* v)
{
   float len = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
   if (len < 1E-6)
      return v;

   v->x /= len;
   v->y /= len;
   v->z /= len;

   return v;
}

vec3f_t* vec3_add(vec3f_t* r, const vec3f_t* a, const vec3f_t* b)
{
   r->x = a->x + b->x;
   r->y = a->y + b->y;
   r->z = a->z + b->z;
   return r;
}

vec3f_t* vec3_sub(vec3f_t* r, const vec3f_t* a, const vec3f_t* b)
{
   r->x = a->x - b->x;
   r->y = a->y - b->y;
   r->z = a->z - b->z;
   return r;
}

vec3f_t* vec3_cross(vec3f_t* r, const vec3f_t* a, const vec3f_t* b)
{
   r->x = a->y * b->z - a->z * b->y;
   r->y = a->z * b->x - a->x * b->z;
   r->z = a->x * b->y - a->y * b->x;
   return r;
}

vec3f_t* vec3_scale(vec3f_t* r, const vec3f_t* a, float scale)
{
   r->x = a->x * scale;
   r->y = a->y * scale;
   r->z = a->z * scale;
   return r;
}

