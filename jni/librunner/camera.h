#pragma once

#include "vector.h"
#include "matrix.h"

typedef struct cam_t
{
   vec3f_t pos;
   vec3f_t up;
   vec3f_t view_dir;

   vec3f_t speed;
   vec3f_t target;

   vec3f_t max_speed;
   vec3f_t acceleration;
   vec3f_t decceleration;

   mat4f_t proj;
   mat4f_t view;
} cam_t;

void cam_init(cam_t* c, float fov, float aspect, float znear, float zfar);
void cam_slide(cam_t* c, const vec3f_t* offset);
void cam_set_pos(cam_t* c, const vec3f_t* pos);
void cam_set_up(cam_t* c, const vec3f_t* up);
void cam_set_view_dir(cam_t* c, const vec3f_t* view_dir);
void cam_look_at(cam_t* c, const vec3f_t* at);
void cam_update(float dt, cam_t* c);

