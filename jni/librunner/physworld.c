#include "physworld.h"
#include <Bullet-C-Api.h>
#include "common.h"

struct physworld_t
{
   plPhysicsSdkHandle sdk;
   plDynamicsWorldHandle handle;
};

struct rigidbody_t { };
struct collshape_t { };

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

static int set_shape_handle(collshape_t** ps, plCollisionShapeHandle handle)
{
   if (handle == NULL)
   {
      LOGE("Unable to create collision shape");
      return -1;
   }

   (*ps) = (collshape_t*)handle;
   return 0;
}

int collshape_create_sphere(collshape_t** ps, float radius)
{
   plCollisionShapeHandle handle = plNewSphereShape(radius);
   return set_shape_handle(ps, handle);
}

int collshape_create_box(collshape_t** ps, float width, float height, float depth)
{
   plCollisionShapeHandle handle = plNewBoxShape(width, height, depth);
   return set_shape_handle(ps, handle);
}

void collshape_free(collshape_t* s)
{
   plDeleteShape((plCollisionShapeHandle)s);
}

int rigidbody_create(rigidbody_t** pb, float mass, collshape_t* s)
{
   plRigidBodyHandle handle = plCreateRigidBody(NULL, mass, (plCollisionShapeHandle)s);
   if (handle == NULL)
   {
      LOGE("Unable to create rigid body");
      return -1;
   }

   (*pb) = (rigidbody_t*)handle;
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

