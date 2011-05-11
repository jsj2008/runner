#include "camera.h"

#define M_EPSILON 0.0001f

void inertion_movement_scalar(float dt, float* porigin, float* pspeed, float target, float max_speed, float acceleration, float decceleration)
{
   float origin = (*porigin);
   float speed = (*pspeed);

   float dist = fabs(target - origin);
   float abs_speed = fabs(speed);
   float target_direction = (target > origin) ? 1.0f : -1.0f;
   float moving_direction = (speed > -M_EPSILON) ? 1.0f : -1.0f;

   if (abs_speed <= M_EPSILON)
   {
      // idling
      moving_direction = target_direction;
      abs_speed = 0.0f;
   }

   if (target_direction == moving_direction)
   {
      // moving in the same direction

      // time & distance to stop
      float tts = abs_speed / decceleration;
      float dts = abs_speed * tts * 0.5f + dt * abs_speed;

      if (dist > dts)
      {
         // we're able to stop in time, so accelerating

         // determine acceleration amount
         float capped_acceleration = acceleration;
         if (capped_acceleration > dist - dts)
         {
            capped_acceleration = dist - dts;
         }

         //printf("dt: %.2f dist: %.6f dts: %.6f accel: %.6f\n", dt, dist, dts, capped_acceleration);
         if (dt * capped_acceleration > M_EPSILON)
         {
            abs_speed += dt * capped_acceleration;
         }
      }
      else
      {
         // deccelerating before target
         //printf("tts: %.4f\tdts: %.4f\n", tts, dts);
         abs_speed -= dt * decceleration;
      }
   }
   else
   {
      // moving in the opposite direction
      abs_speed -= dt * decceleration;
   }

   // check speed range
   if (abs_speed > max_speed)
   {
      // maximum spead reached
      abs_speed = max_speed;
   }
   if (abs_speed <= M_EPSILON)
   {
      // successfully stopped
      abs_speed = 0.0f;
   }

   // move
   speed = abs_speed * moving_direction;
   origin += dt * speed;

   (*porigin) = origin;
   (*pspeed) = speed;
}

void inertion_movement(float dt, vec4f_t* origin, vec4f_t* speed, const vec4f_t* target, const vec4f_t* max_speed, const vec4f_t* acceleration, const vec4f_t* decceleration)
{
   inertion_movement_scalar(dt, &origin->x, &speed->x, target->x, max_speed->x, acceleration->x, decceleration->x);
   inertion_movement_scalar(dt, &origin->y, &speed->y, target->y, max_speed->y, acceleration->y, decceleration->y);
   inertion_movement_scalar(dt, &origin->z, &speed->z, target->z, max_speed->z, acceleration->z, decceleration->z);
}

void cam_init(cam_t* c, float fov, float aspect, float znear, float zfar)
{
   mat4_set_perspective(&c->proj, DEG2RAD(fov), aspect, znear, zfar);
}

void cam_slide(cam_t* c, const vec4f_t* offset)
{
   vec4_add(&c->target, &c->target, offset);
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

void cam_update(float dt, cam_t* c)
{
   inertion_movement(dt, &c->pos, &c->speed, &c->target, &c->max_speed, &c->acceleration, &c->decceleration);

   vec4f_t look_at;
   vec4_add(&look_at, &c->pos, &c->view_dir);
   mat4_set_lookat(&c->view, &c->pos, &look_at, &c->up);
}

