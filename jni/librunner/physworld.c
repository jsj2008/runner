#include "physworld.h"
#include <Bullet-C-Api.h>
#include "common.h"
#include "world.h"
#include "game.h"
#include "shader.h"
#include "resman.h"

struct physworld_t
{
   plPhysicsSdkHandle sdk;
   plDynamicsWorldHandle handle;
};

struct rigidbody_t { };


extern game_t* game;

#define MAX_LINES 8192
int nlines;
vec3f_t vertices[MAX_LINES * 2];
vec3f_t colors[MAX_LINES * 2];

void dd_draw_line(const vec3f_t* from, const vec3f_t* to, const vec3f_t* color)
{
   if (nlines >= MAX_LINES)
      return;

   vertices[nlines * 2] = *from;
   vertices[nlines * 2 + 1] = *to;
   colors[nlines * 2] = *color;
   colors[nlines * 2 + 1] = *color;
   ++nlines;
}

int physworld_create(physworld_t** pw)
{
   physworld_t* w = (physworld_t*)malloc(sizeof(physworld_t));
   memset(w, 0, sizeof(physworld_t));

   w->sdk = plNewBulletSdk();
   if (w->sdk == NULL)
   {
      LOGE("Unable to create bullet sdk instance");
      physworld_free(w);
      return -1;
   }

   w->handle = plCreateDynamicsWorld(w->sdk, (DebugDrawLinePtr)dd_draw_line);
   if (w->handle == NULL)
   {
      LOGE("Unable to create dynamics world");
      physworld_free(w);
      return -1;
   }

   (*pw) = w;
   return 0;
}

void physworld_free(physworld_t* w)
{
   if (w->handle != NULL)
   {
      plDeleteDynamicsWorld(w->handle);
   }
   if (w->sdk != NULL)
   {
      plDeletePhysicsSdk(w->sdk);
   }
   free(w);
}

void physworld_update(physworld_t* w, float dt)
{
   const float internal_dt = 1.0f / 60.0f;
   int steps = (int)(dt / internal_dt + 1.0f);
   plStepSimulationPrecise(w->handle, dt, steps, internal_dt);
}

void physworld_add_rigidbody(physworld_t* w, rigidbody_t* b)
{
   plAddRigidBody(w->handle, (plRigidBodyHandle)b);
}

void physworld_remove_rigidbody(physworld_t* w, rigidbody_t* b)
{
   plRemoveRigidBody(w->handle, (plRigidBodyHandle)b);
}

void physworld_set_gravity(physworld_t* w, const vec3f_t* gravity)
{
   plSetGravity(w->handle, (float*)gravity);
}

void physworld_render(const physworld_t* w, const camera_t* camera)
{
   shader_t* shader = resman_get_shader(game->resman, "shaders/physics");
   if (shader == NULL)
      return;

   nlines = 0;
   plWorldDebugDraw(w->handle);

   mat4f_t mvp;
   mat4_mult(&mvp, &camera->proj, &camera->view);

   shader_use(shader);
   shader_set_uniform_matrices(shader, "uMVP", 1, mat4_data(&mvp));
   shader_set_attrib_vertices(shader, "aPos", 3, GL_FLOAT, 0, &vertices[0]);
   shader_set_attrib_vertices(shader, "aColor", 3, GL_FLOAT, 0, &colors[0]);

   glLineWidth(2);
   glDrawArrays(GL_LINES, 0, nlines * 2);

   shader_unuse(shader);
}

int rigidbody_create(rigidbody_t** pb, const struct phys_t* phys, const struct mesh_t* mesh, transform_setter setter, transform_getter getter, void* user_data)
{
   if (phys->type == PHYS_NOCOLLISION)
   {
      return -1;
   }

   long l = 0;

   const struct shape_t* sp = &phys->shape;
   plCollisionShapeHandle s = NULL;
   switch (sp->type)
   {
   case SHAPE_SPHERE:
      s = plNewSphereShape(sp->radius);
      break;

   case SHAPE_CYLINDER:
      LOGI("CYLINDER: %.2f %.2f", sp->radius, sp->extents.z);
      s = plNewCylinderShape(sp->radius, sp->extents.z / 2.0f);
      break;

   case SHAPE_BOX:
      s = plNewBoxShape(sp->extents.x/2.0f, sp->extents.y/2.0f, sp->extents.z/2.0f);
      break;

   case SHAPE_CONVEX:
   {
      s = plNewConvexHullShape();
      vertex_t* vert = &mesh->vertices[0];
      for (l = 0; l < mesh->nvertices; ++l, ++vert)
      {
         plAddVertex(s, vert->point.x, vert->point.y, vert->point.z);
      }
      break;
   }

   case SHAPE_CONCAVE:
   {
      const struct submesh_t* submesh = &mesh->submeshes[0];
      for (l = 0; l < mesh->nsubmeshes; ++l, ++submesh)
      {
         s = plNewBvhTriangleMeshShape(
                submesh->nindices,
                &submesh->indices[0],
                3*sizeof(int),
                mesh->nvertices,
                (plReal*)&mesh->vertices[0].point,
                sizeof(vertex_t));
      }
      break;
   }

   default:
      LOGE("Unknown shape type: %d", sp->type);
      return -1;
   }

   if (s == NULL)
   {
      LOGE("Unable to create collision shape");
      return -1;
   }

   plSetMargin(s, sp->margin);

   float mass = (phys->type == PHYS_RIGID) ? phys->mass : 0.0f;
   LOGI("MASS: %.2f INERTIA FACTOR: %.2f", mass, phys->inertia_factor);
   LOGI("SLEEPING THRESHOLDS: %.2f %.2f", phys->linear_sleeping_threshold, phys->angular_sleeping_threshold);
   LOGI("FRICTION: %.2f RESTITUTION: %.2f", phys->friction, phys->restitution);

   plRigidBodyHandle handle = plCreateRigidBody(user_data, mass, s, phys->inertia_factor, (motionstate_setter)setter, (motionstate_getter)getter);
   if (handle == NULL)
   {
      LOGE("Unable to create rigid body");
      return -1;
   }

   (*pb) = (rigidbody_t*)handle;

   rigidbody_set_friction(*pb, phys->friction);
   rigidbody_set_restitution(*pb, phys->restitution);
   rigidbody_set_damping(*pb, phys->linear_damping, phys->angular_damping);
   //rigidbody_set_sleeping_thresholds(*pb, phys->linear_sleeping_threshold, phys->angular_sleeping_threshold);
   rigidbody_set_linear_factor(*pb, &phys->linear_factor);
   rigidbody_set_angular_factor(*pb, &phys->angular_factor);

   return 0;
}

void rigidbody_free(rigidbody_t* b)
{
   plDeleteRigidBody((plRigidBodyHandle)b);
}

void rigidbody_set_transform(rigidbody_t* b, const mat4f_t* transform)
{
   mat4f_t tmp = (*transform);
   plSetOpenGLMatrix((plRigidBodyHandle)b, &tmp.m[0]);
}

void rigidbody_get_transform(const rigidbody_t* b, mat4f_t* transform)
{
   plGetOpenGLMatrix((plRigidBodyHandle)b, &transform->m[0]);
}

void rigidbody_set_friction(rigidbody_t* b, float friction)
{
   plSetFriction((plRigidBodyHandle)b, friction);
}

void rigidbody_set_restitution(rigidbody_t* b, float restitution)
{
   plSetRestitution((plRigidBodyHandle)b, restitution);
}

void rigidbody_set_damping(rigidbody_t* b, float linear, float angular)
{
   plSetDamping((plRigidBodyHandle)b, linear, angular);
}

void rigidbody_set_sleeping_thresholds(rigidbody_t* b, float linear, float angular)
{
   plSetSleepingThresholds((plRigidBodyHandle)b, linear, angular);
}

void rigidbody_set_linear_factor(rigidbody_t* b, const vec3f_t* factor)
{
   plSetLinearFactor((plRigidBodyHandle)b, (float*)factor);
}

void rigidbody_set_angular_factor(rigidbody_t* b, const vec3f_t* factor)
{
   plSetAngularFactor((plRigidBodyHandle)b, (float*)factor);
}

void rigidbody_apply_central_impulse(rigidbody_t* b, const vec3f_t* impulse)
{
   plApplyCentralImpulse((plRigidBodyHandle)b, (float*)impulse);
}

