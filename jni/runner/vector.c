#include "vector.h"
#include <math.h>

vec4f_t* vec4_normalize(vec4f_t* v)
{
   float len = sqrt(v->x*v->x + v->y*v->y + v->z*v->z);
   if (len < 1E-6)
      return;

   v->x /= len;
   v->y /= len;
   v->z /= len;
   v->w = 1.0f;

   return v;
}

vec4f_t* vec4_add(vec4f_t* r, const vec4f_t* a, const vec4f_t* b)
{
   r->x = a->x + b->x;
   r->y = a->y + b->y;
   r->z = a->z + b->z;
   return r;
}

vec4f_t* vec4_sub(vec4f_t* r, const vec4f_t* a, const vec4f_t* b)
{
   r->x = a->x - b->x;
   r->y = a->y - b->y;
   r->z = a->z - b->z;
   return r;
}

vec4f_t* vec4_cross(vec4f_t* r, const vec4f_t* a, const vec4f_t* b)
{
   r->x = a->y * b->z - a->z * b->y;
   r->y = a->z * b->x - a->x * b->z;
   r->z = a->x * b->y - a->y * b->x;
   return r;
}

