#pragma once

#include "mathlib.h"

typedef struct physworld_t physworld_t;
typedef struct rigidbody_t rigidbody_t;

typedef struct collshape_params_t
{
   enum shape_type_t
   {
      SHAPE_SPHERE = 0,
      SHAPE_BOX,
      SHAPE_CONVEX,
      SHAPE_BVH_TRIMESH,
   } type;

   union
   {
      struct
      {
         float radius;
      } sphere;

      struct
      {
         float width;
         float height;
         float depth;
      } box;

      struct
      {
         long npoints;
         vec3f_t* points;
      } convex;

      struct
      {
         long nindices;
         long indices_stride;
         int* indices;

         long nvertices;
         long vertices_stride;
         float* vertices;
      } bvh_trimesh;
   } params;
} collshape_params_t;

typedef struct rigidbody_params_t
{
   float mass;
   float friction;
   float restitution;

   float linear_damping;
   float angular_damping;

   float linear_sleeping_threshold;
   float angular_sleeping_threshold;

   mat4f_t transform;

   collshape_params_t shape_params;
} rigidbody_params_t;

int physworld_create(physworld_t** pw);
void physworld_free(physworld_t* w);
void physworld_update(physworld_t* w, float dt);
void physworld_add_rigidbody(physworld_t* w, rigidbody_t* b);
void physworld_remove_rigidbody(physworld_t* w, rigidbody_t* b);
void physworld_set_gravity(physworld_t* w, const vec3f_t* gravity);

int rigidbody_create(rigidbody_t** pb, const rigidbody_params_t* params);
void rigidbody_free(rigidbody_t* b);
void rigidbody_set_transform(rigidbody_t* b, const mat4f_t* transform);
void rigidbody_get_transform(const rigidbody_t* b, mat4f_t* transform);
void rigidbody_apply_central_impulse(rigidbody_t* b, const vec3f_t* impulse);

