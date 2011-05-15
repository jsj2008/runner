#include "vector.h"
#include <math.h>

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

quat_t* quat_from_angles(quat_t* r, const vec3f_t* a)
{
   float ax = a->x * 0.5f;
   float sin0 = sin(ax);
   float cos0 = cos(ax);

   float ay = a->y * 0.5f;
   float sin1 = sin(ay);
   float cos1 = cos(ay);

   float az = a->z * 0.5f;
   float sin2 = sin(az);
   float cos2 = cos(az);

   r->x = sin0 * cos1 * cos2 - cos0 * sin1 * sin2;
   r->y = cos0 * sin1 * cos2 + sin0 * cos1 * sin2;
   r->z = cos0 * cos1 * sin2 - sin0 * sin1 * cos2;
   r->w = cos0 * cos1 * cos2 + sin0 * sin1 * sin2;

   return r;
}

vec3f_t* quat_to_angles(vec3f_t* r, const quat_t* q)
{
   float test = q->x*q->y + q->z*q->w;
   if (test > 0.499)   // singularity at north pole
   {
      r->y = 2 * atan2(q->x,q->w);
      r->z = M_PI/2;
      r->x = 0;
      return r;
   }
   if (test < -0.499)   // singularity at south pole
   {
      r->y = -2 * atan2(q->x,q->w);
      r->z = - M_PI/2;
      r->x = 0;
      return r;
   }
   float sqx = q->x*q->x;
   float sqy = q->y*q->y;
   float sqz = q->z*q->z;
   r->y = atan2(2*q->y*q->w-2*q->x*q->z , 1 - 2*sqy - 2*sqz);
   r->z = asin(2*test);
   r->x = atan2(2*q->x*q->w-2*q->y*q->z , 1 - 2*sqx - 2*sqz);

   return r;
}

