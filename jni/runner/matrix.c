#include "matrix.h"
#include "common.h"

void mat4_set_identity(mat4f_t* m)
{
   memset(m->m, 0, sizeof(m->m));
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
/*
void matSetRotationX(mat4f_t* m, float angle)
{
   float sx = sin(angle);
   float cx = cos(angle);

   memset(m->m, 0, sizeof(m->m));
   m->m00 = m->m33 = 1.0f;
   m->m11 = cx;
   m->m12 = -sx;
   m->m21 = sx;
   m->m22 = cx;
}

void matSetRotationY(mat4f_t* m, float angle)
{
   float sy = sin(angle);
   float cy = cos(angle);

   memset(m->m, 0, sizeof(m->m));
   m->m11 = m->m33 = 1.0f;
   m->m00 = cy;
   m->m02 = sy;
   m->m20 = -sy;
   m->m22 = cy;
}

void matSetRotationZ(mat4f_t* m, float angle)
{
   float sz = sin(angle);
   float cz = cos(angle);

   memset(m->m, 0, sizeof(m->m));
   m->m22 = m->m33 = 1.0f;
   m->m00 = cz;
   m->m01 = -sz;
   m->m10 = sz;
   m->m11 = cz;
}

void matSetRotationXYZ(mat4f_t* m, float xangle, float yangle, float zangle)
{
   float sx = sin(xangle);
   float cx = cos(xangle);
   float sy = sin(yangle);
   float cy = cos(yangle);
   float sz = sin(zangle);
   float cz = cos(zangle);

   m->m00 = cy * cz;
   m->m01 = -cy * sz;
   m->m02 = sy;

   m->m10 = cz * sx * sy + cx * sz;
   m->m11 = cx * cz - sx * sy * sz;
   m->m12 = -cy * sx;

   m->m20 = -cx * cz * sy + sx * sz;
   m->m21 = cz * sx + cx * sy + sz;
   m->m22 = cx * cy;

   m->m33 = 1.0f;
   m->m03 = m->m13 = m->m23 = m->m30 = m->m31 = m->m32 = 0.0f;
}

void matSetView(mat4f_t* m, vec4f_t* from, vec4f_t* view, vec4f_t* worldUp)
{
   vec4f_t up, right;
   float dotProduct = (worldUp->x * view->x) + (worldUp->y * view->y) + (worldUp->z * view->z);

   up.x = worldUp->x - (dotProduct * view->x);
   up.y = worldUp->y - (dotProduct * view->y);
   up.z = worldUp->z - (dotProduct * view->z);
   float length = sqrt((up.x * up.x) + (up.y * up.y) + (up.z * up.z));
   if (1E-6f > length)
   {
      up.x = 0.0f - (view->y * view->x);
      up.y = 1.0f - (view->y * view->y);
      up.z = 0.0f - (view->y * view->z);
      length = sqrt((up.x * up.x) + (up.y * up.y) + (up.z * up.z));
      if (1E-6f > length)
      {
         up.x = 0.0f - (view->z * view->x);
         up.y = 0.0f - (view->z * view->y);
         up.z = 1.0f - (view->z * view->z);
         length = sqrt((up.x * up.x) + (up.y * up.y) + (up.z * up.z));
         if (1E-6f > length)
         {
            return;
         }
      }
   }
   length = 1.0f / length;
   up.x *= length;
   up.y *= length;
   up.z *= length;
   right.x = up.y * view->z - up.z * view->y;
   right.y = up.z * view->x - up.x * view->z;
   right.z = up.x * view->y - up.y * view->x;

   m->m00 = right.x;
   m->m01 = right.y;
   m->m02 = right.z;
   m->m03 = - ((from->x * right.x) + (from->y * right.y) + (from->z * right.z));

   m->m10 = up.x;
   m->m11 = up.y;
   m->m12 = up.z;
   m->m13 = - ((from->x * up.x)    + (from->y * up.y)    + (from->z * up.z));
   m->m20 = view->x;
   m->m21 = view->y;
   m->m22 = view->z;
   m->m23 = - ((from->x * view->x) + (from->y * view->y) + (from->z * view->z));
   m->m30 = m->m31 = m->m32 = 0.0f;
   m->m33 = 1.0f;
}

void matShow(mat4f_t* m)
{
   LOGI("%.2f %.2f %.2f %.2f", m->m00, m->m01, m->m02, m->m03);
   LOGI("%.2f %.2f %.2f %.2f", m->m10, m->m11, m->m12, m->m13);
   LOGI("%.2f %.2f %.2f %.2f", m->m20, m->m21, m->m22, m->m23);
   LOGI("%.2f %.2f %.2f %.2f", m->m30, m->m31, m->m32, m->m33);
}
*/


void mat4_show(mat4f_t* m)
{
   LOGI("%.2f %.2f %.2f %.2f", m->m11, m->m21, m->m31, m->m41);
   LOGI("%.2f %.2f %.2f %.2f", m->m12, m->m22, m->m32, m->m42);
   LOGI("%.2f %.2f %.2f %.2f", m->m13, m->m23, m->m33, m->m43);
   LOGI("%.2f %.2f %.2f %.2f", m->m14, m->m24, m->m34, m->m44);
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

void mat4_set_lookat(mat4f_t* m, const vec4f_t* eye, const vec4f_t* at, const vec4f_t* up)
{
   vec4f_t f, s, u;

   vec4f_t nup = *up;
   vec4_normalize(&nup);

   vec4_normalize(vec4_sub(&f, at, eye));
   vec4_normalize(vec4_cross(&s, &f, &nup));
   vec4_normalize(vec4_cross(&u, &s, &f));

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

float* mat4_data(mat4f_t* m)
{
   return (float*)m->m;
}

