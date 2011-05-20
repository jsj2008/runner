#include "matrix.h"
#include "common.h"

void mat4_set_zero(mat4f_t* m)
{
   memset(m->m, 0, sizeof(m->m));
}

void mat4_set_identity(mat4f_t* m)
{
   mat4_set_zero(m);
   m->m11 = m->m22 = m->m33 = m->m44 = 1.0f;
}

#define SWAP(a, b) { tmp = a; a = b; b = tmp; }

void mat4_transpose(mat4f_t* m)
{
   float tmp;

   SWAP(m->m12, m->m21);
   SWAP(m->m13, m->m31);
   SWAP(m->m14, m->m41);
   SWAP(m->m23, m->m32);
   SWAP(m->m24, m->m42);
   SWAP(m->m34, m->m43);
}

void mat4_transposed(mat4f_t* m, const mat4f_t* a)
{
   memcpy(m, a, sizeof(mat4f_t));
   mat4_transpose(m);
}

float mat4_determinant(const mat4f_t* m)
{
   float det =
      m->m14*m->m23*m->m32*m->m41 - m->m13*m->m24*m->m32*m->m41 - m->m14*m->m22*m->m33*m->m41 + m->m12*m->m24*m->m33*m->m41 +
      m->m13*m->m22*m->m34*m->m41 - m->m12*m->m23*m->m34*m->m41 - m->m14*m->m23*m->m31*m->m42 + m->m13*m->m24*m->m31*m->m42 +
      m->m14*m->m21*m->m33*m->m42 - m->m11*m->m24*m->m33*m->m42 - m->m13*m->m21*m->m34*m->m42 + m->m11*m->m23*m->m34*m->m42 +
      m->m14*m->m22*m->m31*m->m43 - m->m12*m->m24*m->m31*m->m43 - m->m14*m->m21*m->m32*m->m43 + m->m11*m->m24*m->m32*m->m43 +
      m->m12*m->m21*m->m34*m->m43 - m->m11*m->m22*m->m34*m->m43 - m->m13*m->m22*m->m31*m->m44 + m->m12*m->m23*m->m31*m->m44 +
      m->m13*m->m21*m->m32*m->m44 - m->m11*m->m23*m->m32*m->m44 - m->m12*m->m21*m->m33*m->m44 + m->m11*m->m22*m->m33*m->m44;
   return det;
}

void mat4_scale(mat4f_t* m, float scale)
{
   int i = 0;
   for (i = 0; i < 16; ++i)
   {
      m->m[i] *= scale;
   }
}

void mat4_invert(mat4f_t* m)
{
   float det = mat4_determinant(m);

   mat4f_t tmp;
   tmp.m11 = m->m23*m->m34*m->m42 - m->m24*m->m33*m->m42 + m->m24*m->m32*m->m43 - m->m22*m->m34*m->m43 - m->m23*m->m32*m->m44 + m->m22*m->m33*m->m44;
   tmp.m12 = m->m14*m->m33*m->m42 - m->m13*m->m34*m->m42 - m->m14*m->m32*m->m43 + m->m12*m->m34*m->m43 + m->m13*m->m32*m->m44 - m->m12*m->m33*m->m44;
   tmp.m13 = m->m13*m->m24*m->m42 - m->m14*m->m23*m->m42 + m->m14*m->m22*m->m43 - m->m12*m->m24*m->m43 - m->m13*m->m22*m->m44 + m->m12*m->m23*m->m44;
   tmp.m14 = m->m14*m->m23*m->m32 - m->m13*m->m24*m->m32 - m->m14*m->m22*m->m33 + m->m12*m->m24*m->m33 + m->m13*m->m22*m->m34 - m->m12*m->m23*m->m34;
   tmp.m21 = m->m24*m->m33*m->m41 - m->m23*m->m34*m->m41 - m->m24*m->m31*m->m43 + m->m21*m->m34*m->m43 + m->m23*m->m31*m->m44 - m->m21*m->m33*m->m44;
   tmp.m22 = m->m13*m->m34*m->m41 - m->m14*m->m33*m->m41 + m->m14*m->m31*m->m43 - m->m11*m->m34*m->m43 - m->m13*m->m31*m->m44 + m->m11*m->m33*m->m44;
   tmp.m23 = m->m14*m->m23*m->m41 - m->m13*m->m24*m->m41 - m->m14*m->m21*m->m43 + m->m11*m->m24*m->m43 + m->m13*m->m21*m->m44 - m->m11*m->m23*m->m44;
   tmp.m24 = m->m13*m->m24*m->m31 - m->m14*m->m23*m->m31 + m->m14*m->m21*m->m33 - m->m11*m->m24*m->m33 - m->m13*m->m21*m->m34 + m->m11*m->m23*m->m34;
   tmp.m31 = m->m22*m->m34*m->m41 - m->m24*m->m32*m->m41 + m->m24*m->m31*m->m42 - m->m21*m->m34*m->m42 - m->m22*m->m31*m->m44 + m->m21*m->m32*m->m44;
   tmp.m32 = m->m14*m->m32*m->m41 - m->m12*m->m34*m->m41 - m->m14*m->m31*m->m42 + m->m11*m->m34*m->m42 + m->m12*m->m31*m->m44 - m->m11*m->m32*m->m44;
   tmp.m33 = m->m12*m->m24*m->m41 - m->m14*m->m22*m->m41 + m->m14*m->m21*m->m42 - m->m11*m->m24*m->m42 - m->m12*m->m21*m->m44 + m->m11*m->m22*m->m44;
   tmp.m34 = m->m14*m->m22*m->m31 - m->m12*m->m24*m->m31 - m->m14*m->m21*m->m32 + m->m11*m->m24*m->m32 + m->m12*m->m21*m->m34 - m->m11*m->m22*m->m34;
   tmp.m41 = m->m23*m->m32*m->m41 - m->m22*m->m33*m->m41 - m->m23*m->m31*m->m42 + m->m21*m->m33*m->m42 + m->m22*m->m31*m->m43 - m->m21*m->m32*m->m43;
   tmp.m42 = m->m12*m->m33*m->m41 - m->m13*m->m32*m->m41 + m->m13*m->m31*m->m42 - m->m11*m->m33*m->m42 - m->m12*m->m31*m->m43 + m->m11*m->m32*m->m43;
   tmp.m43 = m->m13*m->m22*m->m41 - m->m12*m->m23*m->m41 - m->m13*m->m21*m->m42 + m->m11*m->m23*m->m42 + m->m12*m->m21*m->m43 - m->m11*m->m22*m->m43;
   tmp.m44 = m->m12*m->m23*m->m31 - m->m13*m->m22*m->m31 + m->m13*m->m21*m->m32 - m->m11*m->m23*m->m32 - m->m12*m->m21*m->m33 + m->m11*m->m22*m->m33;

   mat4_scale(&tmp, 1.0f / det);

   memcpy(m, &tmp, sizeof(mat4f_t));
}

void mat4_inverted(mat4f_t* m, const mat4f_t* a)
{
   memcpy(m, a, sizeof(mat4f_t));
   mat4_invert(m);
}

void mat4_set_xrotation(mat4f_t* m, float angle)
{
   float sx = sin(angle);
   float cx = cos(angle);

   memset(m->m, 0, sizeof(m->m));
   m->m11 = m->m44 = 1.0f;
   m->m22 = cx;
   m->m23 = -sx;
   m->m32 = sx;
   m->m33 = cx;
}

void mat4_set_yrotation(mat4f_t* m, float angle)
{
   float sy = sin(angle);
   float cy = cos(angle);

   memset(m->m, 0, sizeof(m->m));
   m->m22 = m->m44 = 1.0f;
   m->m11 = cy;
   m->m13 = sy;
   m->m31 = -sy;
   m->m33 = cy;
}

void mat4_set_zrotation(mat4f_t* m, float angle)
{
   float sz = sin(angle);
   float cz = cos(angle);

   memset(m->m, 0, sizeof(m->m));
   m->m33 = m->m44 = 1.0f;
   m->m11 = cz;
   m->m12 = -sz;
   m->m21 = sz;
   m->m22 = cz;
}

void mat4_show(const mat4f_t* m)
{
   LOGI("%8.2f %8.2f %8.2f %8.2f", m->m11, m->m12, m->m13, m->m14);
   LOGI("%8.2f %8.2f %8.2f %8.2f", m->m21, m->m22, m->m23, m->m24);
   LOGI("%8.2f %8.2f %8.2f %8.2f", m->m31, m->m32, m->m33, m->m34);
   LOGI("%8.2f %8.2f %8.2f %8.2f", m->m41, m->m42, m->m43, m->m44);
}

void mat4_set_translation(mat4f_t* m, float x, float y, float z)
{
   m->m11 = 1.0f;
   m->m12 = 0.0f;
   m->m13 = 0.0f;
   m->m14 = x;

   m->m21 = 0.0f;
   m->m22 = 1.0f;
   m->m23 = 0.0f;
   m->m24 = y;

   m->m31 = 0.0f;
   m->m32 = 0.0f;
   m->m33 = 1.0f;
   m->m34 = z;

   m->m41 = 0.0f;
   m->m42 = 0.0f;
   m->m43 = 0.0f;
   m->m44 = 1.0f;
}

void mat4_set_frustum(mat4f_t* m, float left, float right, float bottom, float top, float znear, float zfar)
{
   float a = (right + left) / ( right - left);
   float b = (top + bottom) / ( top - bottom);
   float c = -(zfar + znear) / (zfar - znear);
   float d = -(2.0f*zfar*znear) / (zfar - znear);

   m->m11 = 2.0f * znear / (right - left);
   m->m12 = 0.0f;
   m->m13 = a;
   m->m14 = 0.0f;

   m->m21 = 0.0f;
   m->m22 = 2.0f * znear / (top - bottom);
   m->m23 = b;
   m->m24 = 0.0f;

   m->m31 = 0.0f;
   m->m32 = 0.0f;
   m->m33 = c;
   m->m34 = d;

   m->m41 = 0.0f;
   m->m42 = 0.0f;
   m->m43 = -1.0f;
   m->m44 = 0.0f;
}

void mat4_set_perspective(mat4f_t* m, float fovy, float aspect, float znear, float zfar)
{
   float ymax = znear * tan (fovy / 2.0f);
   float ymin = -ymax;
   float xmin = ymin * aspect;
   float xmax = ymax * aspect;
   mat4_set_frustum(m, xmin, xmax, ymin, ymax, znear, zfar);
}

void mat4_set_lookat(mat4f_t* m, const vec3f_t* eye, const vec3f_t* at, const vec3f_t* up)
{
   vec3f_t f, s, u;

   vec3f_t nup = *up;
   vec3_normalize(&nup);

   vec3_normalize(vec3_sub(&f, at, eye));
   vec3_normalize(vec3_cross(&s, &f, &nup));
   vec3_normalize(vec3_cross(&u, &s, &f));

   mat4f_t r;
   r.m11 = s.x;
   r.m12 = s.y;
   r.m13 = s.z;
   r.m14 = 0.0f;

   r.m21 = u.x;
   r.m22 = u.y;
   r.m23 = u.z;
   r.m24 = 0.0f;

   r.m31 = -f.x;
   r.m32 = -f.y;
   r.m33 = -f.z;
   r.m34 = 0.0f;

   r.m41 = 0.0f;
   r.m42 = 0.0f;
   r.m43 = 0.0f;
   r.m44 = 1.0f;

   mat4f_t t;
   mat4_set_translation(&t, -eye->x, -eye->y, -eye->z);

   mat4_mult(m, &r, &t);
}

void mat4_mult(mat4f_t* m, const mat4f_t* a, const mat4f_t* b)
{
   m->m11 = a->m11*b->m11 + a->m12*b->m21 + a->m13*b->m31 + a->m14*b->m41;
   m->m12 = a->m11*b->m12 + a->m12*b->m22 + a->m13*b->m32 + a->m14*b->m42;
   m->m13 = a->m11*b->m13 + a->m12*b->m23 + a->m13*b->m33 + a->m14*b->m43;
   m->m14 = a->m11*b->m14 + a->m12*b->m24 + a->m13*b->m34 + a->m14*b->m44;

   m->m21 = a->m21*b->m11 + a->m22*b->m21 + a->m23*b->m31 + a->m24*b->m41;
   m->m22 = a->m21*b->m12 + a->m22*b->m22 + a->m23*b->m32 + a->m24*b->m42;
   m->m23 = a->m21*b->m13 + a->m22*b->m23 + a->m23*b->m33 + a->m24*b->m43;
   m->m24 = a->m21*b->m14 + a->m22*b->m24 + a->m23*b->m34 + a->m24*b->m44;

   m->m31 = a->m31*b->m11 + a->m32*b->m21 + a->m33*b->m31 + a->m34*b->m41;
   m->m32 = a->m31*b->m12 + a->m32*b->m22 + a->m33*b->m32 + a->m34*b->m42;
   m->m33 = a->m31*b->m13 + a->m32*b->m23 + a->m33*b->m33 + a->m34*b->m43;
   m->m34 = a->m31*b->m14 + a->m32*b->m24 + a->m33*b->m34 + a->m34*b->m44;

   m->m41 = a->m41*b->m11 + a->m42*b->m21 + a->m43*b->m31 + a->m44*b->m41;
   m->m42 = a->m41*b->m12 + a->m42*b->m22 + a->m43*b->m32 + a->m44*b->m42;
   m->m43 = a->m41*b->m13 + a->m42*b->m23 + a->m43*b->m33 + a->m44*b->m43;
   m->m44 = a->m41*b->m14 + a->m42*b->m24 + a->m43*b->m34 + a->m44*b->m44;
}

const float* mat4_data(const mat4f_t* m)
{
   return (const float*)m->m;
}

void mat4_from_quaternion(mat4f_t* m, const quat_t* q)
{
   float xx = q->x * q->x;
   float xy = q->x * q->y;
   float xz = q->x * q->z;
   float xw = q->x * q->w;

   float yy = q->y * q->y;
   float yz = q->y * q->z;
   float yw = q->y * q->w;

   float zz = q->z * q->z;
   float zw = q->z * q->w;

   m->m11 = 1.0f - 2.0f * (yy + zz);
   m->m21 = 2.0f * (xy + zw);
   m->m31 = 2.0f * (xz - yw);
   m->m41 = 0.0f;

   m->m12 = 2.0f * (xy - zw);
   m->m22 = 1.0f - 2.0f * (xx + zz);
   m->m32 = 2.0f * (yz + xw);
   m->m42 = 0.0f;

   m->m13 = 2.0f * (xz + yw);
   m->m23 = 2.0f * (yz - xw);
   m->m33 = 1.0f - 2.0f * (xx + yy);
   m->m43 = 0.0f;

   m->m14 = 0.0f;
   m->m24 = 0.0f;
   m->m34 = 0.0f;
   m->m44 = 1.0f;
}

void mat4_mult_vec3(vec3f_t* r, const mat4f_t* m, const vec3f_t* a)
{
   r->x = m->m11 * a->x + m->m12 * a->y + m->m13 * a->z + m->m14;
   r->y = m->m21 * a->x + m->m22 * a->y + m->m23 * a->z + m->m24;
   r->z = m->m31 * a->x + m->m32 * a->y + m->m33 * a->z + m->m34;
}

void mat4_mult_vec4(vec4f_t* r, const mat4f_t* m, const vec4f_t* a)
{
   r->x = m->m11 * a->x + m->m12 * a->y + m->m13 * a->z + m->m14 * a->w;
   r->y = m->m21 * a->x + m->m22 * a->y + m->m23 * a->z + m->m24 * a->w;
   r->z = m->m31 * a->x + m->m32 * a->y + m->m33 * a->z + m->m34 * a->w;
   r->w = m->m41 * a->x + m->m42 * a->y + m->m43 * a->z + m->m44 * a->w;
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

