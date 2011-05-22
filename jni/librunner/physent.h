#pragma once

#include "mathlib.h"

// wcs - world coordinate system
// bcs - body coordinate system

typedef struct physent_t
{
   vec3f_t position;
   quat_t orientation;

   float mass;
   mat4f_t inertia_bcs;

   vec3f_t velocity;
   vec3f_t angular_velocity;

   vec3f_t force;
   vec3f_t torque;
} physent_t;

void physent_apply_force_wcs(physent_t* e, const vec3f_t* point, const vec3f_t* force);
void physent_apply_force_bcs(physent_t* e, const vec3f_t* point, const vec3f_t* force);
void physent_set_sphere_inertia(physent_t* e, float radius);
void physent_set_box_inertia(physent_t* e, float width, float height, float depth);
void physent_get_transform(const physent_t* e, mat4f_t* transform);
void physent_update(physent_t* e, float dt);
void physent_show(const physent_t* e);

