#pragma once

#include "vector.h"
#include "matrix.h"

typedef struct cam_t
{
   vec4f_t pos;
   vec4f_t up;
   vec4f_t view_dir;

   mat4f_t proj;
   mat4f_t view;
} cam_t;

void cam_init(cam_t* c, float fov, float aspect, float znear, float zfar);
void cam_slide(cam_t* c, const vec4f_t* offset);
void cam_set_pos(cam_t* c, const vec4f_t* pos);
void cam_set_up(cam_t* c, const vec4f_t* up);
void cam_set_view_dir(cam_t* c, const vec4f_t* view_dir);
void cam_look_at(cam_t* c, const vec4f_t* at);
void cam_update(cam_t* c);

