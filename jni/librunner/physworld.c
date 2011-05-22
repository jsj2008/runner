#include "physworld.h"
#include <Bullet-C-Api.h>
#include "common.h"

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
   plStepSimulation(w->handle, dt);
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

int rigidbody_create(rigidbody_t** pb, const rigidbody_params_t* params)
{
   const collshape_params_t* sp = &params->shape_params;
   plCollisionShapeHandle s = NULL;
   switch (sp->type)
   {
      case SHAPE_SPHERE:
         s = plNewSphereShape(sp->params.sphere.radius);
         break;

      case SHAPE_BOX:
         s = plNewBoxShape(sp->params.box.width/2.0f, sp->params.box.height/2.0f, sp->params.box.width/2.0f);
         break;

      default:
         LOGE("Unknown shape type: %d", sp->type);
         return -1;
   }

   if (s == NULL)
   {
      LOGE("Unable to create collision shape");
      return -1;
   }

   plRigidBodyHandle handle = plCreateRigidBody(NULL, params->mass, s);
   if (handle == NULL)
   {
      LOGE("Unable to create rigid body");
      return -1;
   }

   (*pb) = (rigidbody_t*)handle;

   rigidbody_set_transform(*pb, &params->transform);

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

void rigidbody_apply_central_impulse(rigidbody_t* b, const vec3f_t* impulse)
{
   plApplyCentralImpulse((plRigidBodyHandle)b, (float*)impulse);
}

