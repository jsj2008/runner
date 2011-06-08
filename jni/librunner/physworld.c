#include "physworld.h"
#include <Bullet-C-Api.h>
#include "common.h"
#include "world.h"

struct physworld_t
{
   plPhysicsSdkHandle sdk;
   plDynamicsWorldHandle handle;
};

struct rigidbody_t { };

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

   w->handle = plCreateDynamicsWorld(w->sdk);
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
   int steps = 5;//(int)(dt / internal_dt + 0.5f);
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

int rigidbody_create(rigidbody_t** pb, const struct phys_t* phys, const mesh_t* mesh, const mat4f_t* transform)
{
   if (phys->type == PHYS_NOCOLLISION)
   {
      return -1;
   }

   const struct shape_t* sp = &phys->shape;
   plCollisionShapeHandle s = NULL;
   switch (sp->type)
   {
   case SHAPE_SPHERE:
      s = plNewSphereShape(sp->radius);
      break;

   case SHAPE_BOX:
      s = plNewBoxShape(sp->extents.x/2.0f, sp->extents.y/2.0f, sp->extents.z/2.0f);
      break;

   case SHAPE_CONVEX:
      s = plNewConvexHullShape();
      long l = 0;
      long k = 0;

      const struct submesh_t* submesh = &mesh->submeshes[0];
      for (l = 0; l < mesh->nsubmeshes; ++l, ++submesh)
      {
         vertex_t* vert = &submesh->vertices[0];
         for (k = 0; k < submesh->nvertices; ++k, ++vert)
         {
            plAddVertex(s, vert->point.x, vert->point.y, vert->point.z);
         }
      }
      break;

      /*case SHAPE_CONCAVE:
         s = plNewBvhTriangleMeshShape(
                sp->params.bvh_trimesh.nindices,
                sp->params.bvh_trimesh.indices,
                sp->params.bvh_trimesh.indices_stride,
                sp->params.bvh_trimesh.nvertices,
                sp->params.bvh_trimesh.vertices,
                sp->params.bvh_trimesh.vertices_stride);
         break;*/

   default:
      LOGE("Unknown shape type: %d", sp->type);
      return -1;
   }

   if (s == NULL)
   {
      LOGE("Unable to create collision shape");
      return -1;
   }

   float mass = (phys->type == PHYS_RIGID) ? phys->mass : 0.0f;

   plRigidBodyHandle handle = plCreateRigidBody(NULL, mass, s);
   if (handle == NULL)
   {
      LOGE("Unable to create rigid body");
      return -1;
   }

   (*pb) = (rigidbody_t*)handle;

   rigidbody_set_transform(*pb, transform);
   rigidbody_set_friction(*pb, phys->friction);
   rigidbody_set_restitution(*pb, phys->restitution);
   rigidbody_set_damping(*pb, phys->linear_damping, phys->angular_damping);
   //rigidbody_set_sleeping_thresholds(*pb, phys->linear_sleeping_threshold, phys->angular_sleeping_threshold);

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

void rigidbody_apply_central_impulse(rigidbody_t* b, const vec3f_t* impulse)
{
   plApplyCentralImpulse((plRigidBodyHandle)b, (float*)impulse);
}

