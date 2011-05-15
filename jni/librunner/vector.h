#pragma once

typedef struct vec3f_t
{
   float x;
   float y;
   float z;
} vec3f_t;

typedef struct vec4f_t
{
   float x;
   float y;
   float z;
   float w;
} vec4f_t;

typedef vec4f_t quat_t;

vec3f_t* vec3_normalize(vec3f_t* v);
vec3f_t* vec3_add(vec3f_t* r, const vec3f_t* a, const vec3f_t* b);
vec3f_t* vec3_sub(vec3f_t* r, const vec3f_t* a, const vec3f_t* b);
vec3f_t* vec3_cross(vec3f_t* r, const vec3f_t* a, const vec3f_t* b);
quat_t* quat_from_angles(quat_t* r, const vec3f_t* a);
vec3f_t* quat_to_angles(vec3f_t* r, const quat_t* q);

