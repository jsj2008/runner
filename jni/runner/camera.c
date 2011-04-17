#include "camera.h"

void cam_init(cam_t* c, float fov, float aspect, float znear, float zfar)
{
   mat4_set_perspective(&c->proj, DEG2RAD(fov), aspect, znear, zfar);
}

void cam_slide(cam_t* c, const vec4f_t* offset)
{
   vec4_add(&c->pos, &c->pos, offset);
}

void cam_set_pos(cam_t* c, const vec4f_t* pos)
{
   c->pos = *pos;
}

void cam_set_up(cam_t* c, const vec4f_t* up)
{
   c->up = *up;
   c->up.w = 0.0f;
   vec4_normalize(&c->up);
}

void cam_set_view_dir(cam_t* c, const vec4f_t* view_dir)
{
   c->view_dir = *view_dir;
   c->view_dir.w = 0.0f;
   vec4_normalize(&c->view_dir);
}

void cam_look_at(cam_t* c, const vec4f_t* at)
{
   vec4_sub(&c->view_dir, at, &c->pos);
   c->view_dir.w = 0.0f;
   vec4_normalize(&c->view_dir);
}

void cam_update(cam_t* c)
{
   vec4f_t look_at;
   vec4_add(&look_at, &c->pos, &c->view_dir);
   mat4_set_lookat(&c->view, &c->pos, &look_at, &c->up);
}

