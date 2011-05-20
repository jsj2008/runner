#pragma once

#include <math.h>
#include "vector.h"

#define DEG2RAD(a)   ((a)*M_PI/180.0f)
#define RAD2DEG(a)   ((a)*180.0f/M_PI)

typedef union
{
   float m[16];
   struct
   {
      float m11, m21, m31, m41;
      float m12, m22, m32, m42;
      float m13, m23, m33, m43;
      float m14, m24, m34, m44;
   };
} mat4f_t;

void mat4_set_zero(mat4f_t* m);
void mat4_set_identity(mat4f_t* m);
void mat4_set_xrotation(mat4f_t* m, float angle);
void mat4_set_yrotation(mat4f_t* m, float angle);
void mat4_set_zrotation(mat4f_t* m, float angle);
void mat4_transpose(mat4f_t* m);
void mat4_transposed(mat4f_t* m, const mat4f_t* a);
float mat4_determinant(const mat4f_t* m);
void mat4_scale(mat4f_t* m, float scale);
void mat4_invert(mat4f_t* m);
void mat4_inverted(mat4f_t* m, const mat4f_t* a);
void mat4_mult(mat4f_t* m, const mat4f_t* a, const mat4f_t* b);
void mat4_mult_vec3(vec3f_t* r, const mat4f_t* m, const vec3f_t* a);
void mat4_mult_vec4(vec4f_t* r, const mat4f_t* m, const vec4f_t* a);
void mat4_set_translation(mat4f_t* m, float x, float y, float z);
void mat4_show(const mat4f_t* m);
void mat4_set_frustum(mat4f_t* m, float left, float right, float bottom, float top, float znear, float zfar);
void mat4_set_perspective(mat4f_t* m, float fovy, float aspect, float znear, float zfar);
void mat4_set_lookat(mat4f_t* m, const vec3f_t* eye, const vec3f_t* at, const vec3f_t* up);
const float* mat4_data(const mat4f_t* m);
void mat4_from_quaternion(mat4f_t* m, const quat_t* q);

void quat_mult(quat_t* r, const quat_t* a, const quat_t* b);
void quat_slerp(quat_t* r, const quat_t* a, const quat_t*b, float t);
void quat_inv(quat_t* r, const quat_t* a);
void quat_conjugate(quat_t* r, const quat_t* a);
void quat_scale(quat_t* r, const quat_t* a, float scale);
void quat_add(quat_t* r, const quat_t* a, const quat_t* b);

