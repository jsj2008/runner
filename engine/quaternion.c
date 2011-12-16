#include "mathlib.h"
#include "common.h"

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

void quat_mult(quat_t* r, const quat_t* a, const quat_t* b)
{
   (*r).w = a->w*b->w - a->x*b->x - a->y*b->y - a->z*b->z;
   (*r).x = a->w*b->x + a->x*b->w + a->y*b->z - a->z*b->y;
   (*r).y = a->w*b->y - a->x*b->z + a->y*b->w + a->z*b->x;
   (*r).z = a->w*b->z + a->x*b->y - a->y*b->x + a->z*b->w;
}

void quat_inv(quat_t* r, const quat_t* a)
{
   // TODO: it works only for unit quaternion
   r->x = -a->x;
   r->y = -a->y;
   r->z = -a->z;
   r->w = a->w;
}

void quat_conjugate(quat_t* r, const quat_t* a)
{
   r->x = -a->x;
   r->y = -a->y;
   r->z = -a->z;
   r->w = a->w;
}

void quat_slerp(quat_t* r, const quat_t* a, const quat_t* b, float t)
{
   float dot = a->x * b->x + a->y * b->y + a->z * b->z + a->w * b->w;

   quat_t tmp;
   if (dot < 0.0f)
   {
      dot = -dot;
      quat_inv(&tmp, b);
   }
   else
   {
      tmp = *b;
   }

   float w1;
   float w2;

   if (dot < 0.95f)
   {
      float angle = acos(dot);
      float sinangle = sin(angle);
      w1 = sin((1.0f - t) * angle) / sinangle;
      w2 = sin(t * angle) / sinangle;
   }
   else
   {
      w1 = 1.0f - t;
      w2 = t;
   }

   r->x = a->x * w1 + b->x * w2;
   r->y = a->y * w1 + b->y * w2;
   r->z = a->z * w1 + b->z * w2;
   r->w = a->w * w1 + b->w * w2;
}

void quat_scale(quat_t* r, const quat_t* a, float scale)
{
   r->x = a->x * scale;
   r->y = a->y * scale;
   r->z = a->z * scale;
   r->w = a->w * scale;
}

void quat_add(quat_t* r, const quat_t* a, const quat_t* b)
{
   r->x = a->x + b->x;
   r->y = a->y + b->y;
   r->z = a->z + b->z;
   r->w = a->w + b->w;
}

quat_t* quat_from_matrix(quat_t* r, const mat4f_t* m)
{
   float trace = m->m11 + m->m22 + m->m33;
   if (trace > 0.0f)
   {
      float s = 0.5f / sqrtf(trace + 1.0f);
      r->w = 0.25f / s;
      r->x = (m->m32 - m->m23) * s;
      r->y = (m->m13 - m->m31) * s;
      r->z = (m->m21 - m->m12) * s;
   }
   else
   {
      if (m->m11 > m->m22 && m->m11 > m->m33)
      {
         float s = 2.0f * sqrtf(1.0f + m->m11 - m->m22 - m->m33);
         r->w = (m->m32 - m->m23) / s;
         r->x = 0.25f * s;
         r->y = (m->m12 + m->m21) / s;
         r->z = (m->m13 + m->m31) / s;
      }
      else if (m->m22 > m->m33)
      {
         float s = 2.0f * sqrtf(1.0f + m->m22 - m->m11 - m->m33);
         r->w = (m->m13 - m->m31) / s;
         r->x = (m->m12 + m->m21) / s;
         r->y = 0.25f * s;
         r->z = (m->m23 + m->m32) / s;
      }
      else
      {
         float s = 2.0f * sqrtf(1.0f + m->m33 - m->m11 - m->m22);
         r->w = (m->m21 - m->m12) / s;
         r->x = (m->m13 + m->m31) / s;
         r->y = (m->m23 + m->m32) / s;
         r->z = 0.25f * s;
      }
   }
   return r;
}

void quat_show(const quat_t* q)
{
   LOGI("[%.2f, %.2f, %.2f, %.2f]", q->x, q->y, q->z, q->w);
}
