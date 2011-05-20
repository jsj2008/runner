#include "physent.h"
#include "common.h"

void physent_apply_force_wcs(physent_t* e, const vec3f_t* point, const vec3f_t* force)
{
   // calculate linear force
   vec3_add(&e->force, &e->force, force);

   // calculate torque
   vec3f_t dist;
   vec3f_t tmp;

   vec3_sub(&dist, point, &e->position);
   vec3_cross(&tmp, &dist, force);
   vec3_add(&e->torque, &e->torque, &tmp);
}

void physent_apply_force_bcs(physent_t* e, const vec3f_t* point, const vec3f_t* force)
{
   // TODO
}

void physent_set_sphere_inertia(physent_t* e, float radius)
{
   float val = 0.4f * e->mass * radius * radius;

   mat4f_t inertia;
   mat4_set_zero(&inertia);
   inertia.m11 = inertia.m22 = inertia.m33 = val;
   inertia.m44 = 1.0f;

   e->inertia_bcs = inertia;
}

void physent_set_box_inertia(physent_t* e, float width, float height, float depth)
{
   float val = 1.0f / 12.0f * e->mass;

   mat4f_t inertia;
   mat4_set_zero(&inertia);
   inertia.m11 = val * (height * height + depth * depth);
   inertia.m22 = val * (width * width + depth * depth);
   inertia.m33 = val * (width * width + height * height);
   inertia.m44 = 1.0f;

   e->inertia_bcs = inertia;
}

void physent_get_transform(const physent_t* e, mat4f_t* transform)
{
   mat4_from_quaternion(transform, &e->orientation);
   transform->m14 = e->position.x;
   transform->m24 = e->position.y;
   transform->m34 = e->position.z;
   transform->m44 = 1.0f;
}

void physent_update(physent_t* e, float dt)
{
   float invMass = 1.0f / e->mass;
   mat4f_t orient;
   mat4f_t orient_trans;
   mat4f_t inertia_bcs_inv;
   mat4f_t inertia_wcs_inv;
   mat4f_t mtmp;

   // calc orientation matrices
   mat4_from_quaternion(&orient, &e->orientation);
   mat4_transposed(&orient_trans, &orient);

   // calc inverted inertia tensor in bcs
   mat4_inverted(&inertia_bcs_inv, &e->inertia_bcs);

   // calc inverted inertia tensor in wcs
   mat4_mult(&mtmp, &orient, &inertia_bcs_inv);
   mat4_mult(&inertia_wcs_inv, &mtmp, &orient_trans);

   // integrate linear velocity
   vec3f_t delta;
   vec3_scale(&delta, &e->force, dt * invMass);
   vec3_add(&e->velocity, &e->velocity, &delta);

   // integrate angular velocity
   vec3f_t vtmp;
   mat4_mult_vec3(&vtmp, &inertia_wcs_inv, &e->torque);
   vec3_scale(&delta, &vtmp, dt);
   vec3_add(&e->angular_velocity, &e->angular_velocity, &delta);

   // move
   vec3_scale(&delta, &e->velocity, dt);
   vec3_add(&e->position, &e->position, &delta);

   // rotate
   quat_t q;
   q.x = e->angular_velocity.x;
   q.y = e->angular_velocity.y;
   q.z = e->angular_velocity.z;
   q.w = 0.0f;

   quat_t dq;
   quat_mult(&dq, &q, &e->orientation);
   quat_scale(&dq, &dq, 0.5f * dt);
   quat_add(&e->orientation, &e->orientation, &dq);

   // reset force accumulators
   memset(&e->force, 0, sizeof(vec3f_t));
   memset(&e->torque, 0, sizeof(vec3f_t));
}

void physent_show(const physent_t* e)
{
   LOGI("Position: %.2f %.2f %.2f", e->position.x, e->position.y, e->position.z);
   LOGI("Orient:   %.2f %.2f %.2f %.2f", e->orientation.x, e->orientation.y, e->orientation.z, e->orientation.w);
   LOGI("Velocity: %.2f %.2f %.2f", e->velocity.x, e->velocity.y, e->velocity.z);
   LOGI("AngularV: %.2f %.2f %.2f", e->angular_velocity.x, e->angular_velocity.y, e->angular_velocity.z);
}

