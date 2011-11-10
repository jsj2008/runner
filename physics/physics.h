#pragma once

#include <mathlib.h>

struct physics_world_t;
struct physics_rigid_body_t;
struct physics_shape_t;

struct phys_t;
struct mesh_t;

#ifdef __cplusplus
extern "C" { 
#endif

   typedef void (*motionstate_getter)(const struct physics_rigid_body_t* body, mat4f_t* transform, void* user_data);
   typedef void (*motionstate_setter)(const struct physics_rigid_body_t* body, const mat4f_t* transform, void* user_data);
   typedef void (*physics_debug_draw_line)(const struct vec3f_t* from, const vec3f_t* to, const vec3f_t* color);

   int physics_world_create(struct physics_world_t** pworld, const vec3f_t* aabbMin, const vec3f_t* aabbMax, physics_debug_draw_line drawLine);
   void physics_world_delete(struct physics_world_t* world);
   void physics_world_add_rigid_body(struct physics_world_t* world, struct physics_rigid_body_t* body);
   void physics_world_remove_rigid_body(struct physics_world_t* world, struct physics_rigid_body_t* body);
   void physics_world_step(struct physics_world_t* world, float timeStep, int maxSteps, float internalTimeStep);
   void physics_world_set_gravity(struct physics_world_t* world, const vec3f_t* gravity);
   void physics_world_debug_draw(const struct physics_world_t* world);

   int physics_rigid_body_create(struct physics_rigid_body_t** pbody, const struct phys_t* props, struct mesh_t* mesh, motionstate_setter setter, motionstate_getter getter, void* user_data);
   void physics_rigid_body_delete(struct physics_rigid_body_t* body);
   void physics_rigid_body_apply_central_impulse(struct physics_rigid_body_t* body, const struct vec3f_t* impulse);
   void physics_rigid_body_get_transform(struct physics_rigid_body_t* body, mat4f_t* transform);
   void physics_rigid_body_set_transform(struct physics_rigid_body_t* body, const mat4f_t* transform);
   void physics_rigid_body_set_friction(struct physics_rigid_body_t* body, float friction);
   void physics_rigid_body_set_restitution(struct physics_rigid_body_t* body, float restitute);
   void physics_rigid_body_set_damping(struct physics_rigid_body_t* body, float linear, float angular);
   void physics_rigid_body_set_sleeping_thresholds(struct physics_rigid_body_t* body, float linear, float angular);
   void physics_rigid_body_set_factors(struct physics_rigid_body_t* body, const struct vec3f_t* linear, const struct vec3f_t* angular);

   int physics_shape_create_box(struct physics_shape_t** pshape, float x, float y, float z);
   int physics_shape_create_capsule(struct physics_shape_t** pshape, float radius, float height);
   int physics_shape_create_cone(struct physics_shape_t** pshape, float radius, float height);
   int physics_shape_create_cylinder(struct physics_shape_t** pshape, float radius, float height);
   int physics_shape_create_sphere(struct physics_shape_t** pshape, float radius);
   int physics_shape_create_compound(struct physics_shape_t** pshape);
   int physics_shape_create_convex(struct physics_shape_t** pshape, const struct vec3f_t* vertices, int nvertices, long stride);
   int physics_shape_create_concave(struct physics_shape_t** pshape, const struct vec3f_t* vertices, long nvertices, long vertices_stride, const unsigned int* indices, long nindices, long indices_stride);
   void physics_shape_delete(struct physics_shape_t* shape);
   int physics_shape_add(struct physics_shape_t* shape, struct physics_shape_t* child, const struct vec3f_t* position, const quat_t* orientation);
   void physics_shape_set_margin(struct physics_shape_t* shape, float margin);

#ifdef __cplusplus
}
#endif

