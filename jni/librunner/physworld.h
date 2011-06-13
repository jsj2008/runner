#pragma once

#include "mathlib.h"

typedef struct physworld_t physworld_t;
typedef struct rigidbody_t rigidbody_t;

struct phys_t;
struct mesh_t;
struct camera_t;

int physworld_create(physworld_t** pw);
void physworld_free(physworld_t* w);
void physworld_update(physworld_t* w, float dt);
void physworld_add_rigidbody(physworld_t* w, rigidbody_t* b);
void physworld_remove_rigidbody(physworld_t* w, rigidbody_t* b);
void physworld_set_gravity(physworld_t* w, const vec3f_t* gravity);
void physworld_render(const physworld_t* w, const struct camera_t* camera);

typedef void (*transform_getter)(const rigidbody_t* b, mat4f_t* transform, void* user_data);
typedef void (*transform_setter)(const rigidbody_t* b, const mat4f_t* transform, void* user_data);

int rigidbody_create(rigidbody_t** pb, const struct phys_t* phys, const struct mesh_t* mesh, transform_setter setter, transform_getter getter, void* user_data);
void rigidbody_free(rigidbody_t* b);
void rigidbody_set_transform(rigidbody_t* b, const mat4f_t* transform);
void rigidbody_get_transform(const rigidbody_t* b, mat4f_t* transform);
void rigidbody_set_friction(rigidbody_t* b, float friction);
void rigidbody_set_restitution(rigidbody_t* b, float restitution);
void rigidbody_set_damping(rigidbody_t* b, float linear, float angular);
void rigidbody_set_sleeping_thresholds(rigidbody_t* b, float linear, float angular);
void rigidbody_set_linear_factor(rigidbody_t* b, const vec3f_t* factor);
void rigidbody_set_angular_factor(rigidbody_t* b, const vec3f_t* factor);
void rigidbody_apply_central_impulse(rigidbody_t* b, const vec3f_t* impulse);

