#pragma once

typedef struct vec4f_t
{
   float x;
   float y;
   float z;
   float w;
} vec4f_t;

vec4f_t* vec4_normalize(vec4f_t* v);
vec4f_t* vec4_add(vec4f_t* r, const vec4f_t* a, const vec4f_t* b);
vec4f_t* vec4_sub(vec4f_t* r, const vec4f_t* a, const vec4f_t* b);
vec4f_t* vec4_cross(vec4f_t* r, const vec4f_t* a, const vec4f_t* b);
vec4f_t* quat_from_angles(vec4f_t* r, const vec4f_t* a);
vec4f_t* quat_to_angles(vec4f_t* r, const vec4f_t* q);

