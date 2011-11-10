#include "physics.h"
#include <world.h>
#include <common.h>
#include <btBulletDynamicsCommon.h>

inline btVector3 vc(const vec3f_t* v)
{
   return btVector3(v->x, v->y, v->z);
}

class MotionStateProxy : public btMotionState
{
   motionstate_setter mSetter;
   motionstate_getter mGetter;
   void* mUserData;

public:
   physics_rigid_body_t* mBody;

public:
   MotionStateProxy(motionstate_setter setter, motionstate_getter getter, void* user_data)
   {
      mSetter = setter;
      mGetter = getter;
      mUserData = user_data;
   }

   virtual void getWorldTransform(btTransform &worldTrans) const
   {
      mat4f_t transform;
      (*mGetter)(mBody, &transform, mUserData);
      worldTrans.setFromOpenGLMatrix(transform.m);
   }

   virtual void setWorldTransform(const btTransform &worldTrans)
   {
      mat4f_t transform;
      worldTrans.getOpenGLMatrix(transform.m);
      (*mSetter)(mBody, &transform, mUserData);
   }
};

class DebugDrawer : public btIDebugDraw
{
public:
   DebugDrawer(physics_debug_draw_line _drawLine)
      : mDebugMode (btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb | btIDebugDraw::DBG_DrawContactPoints)
      , mDrawLine (_drawLine)
   { }

   virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color)
   {
      vec3f_t vfrom = { from[0], from[1], from[2] };
      vec3f_t vto = { to[0], to[1], to[2] };
      vec3f_t vcolor = { color[0], color[1], color[2] };
      mDrawLine(&vfrom, &vto, &vcolor);
   }

   virtual void draw3dText(const btVector3& location,const char* textString)
   {
   }

   virtual void drawContactPoint(const btVector3& pointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
   {
      drawLine(pointOnB, pointOnB + normalOnB, color);
   }

   virtual int getDebugMode() const
   {
      return mDebugMode;
   }

   virtual void setDebugMode(int debugMode)
   {
      mDebugMode = debugMode;
   }

   virtual void reportErrorWarning(const char* warningString)
   {
      printf(warningString);
   }

private:
   int mDebugMode;
   physics_debug_draw_line mDrawLine;
};

int physics_world_create(struct physics_world_t** pworld, const vec3f_t* aabbMin, const vec3f_t* aabbMax, physics_debug_draw_line drawLine)
{
   void* mem = btAlignedAlloc(sizeof(btDefaultCollisionConfiguration), 16);
   btDefaultCollisionConfiguration* collisionConfiguration = new (mem)btDefaultCollisionConfiguration();

   mem = btAlignedAlloc(sizeof(btCollisionDispatcher), 16);
   btDispatcher* dispatcher = new (mem) btCollisionDispatcher(collisionConfiguration);

   mem = btAlignedAlloc(sizeof(btAxisSweep3), 16);
   btBroadphaseInterface* pairCache = new (mem) btAxisSweep3(vc(aabbMin), vc(aabbMax));

   mem = btAlignedAlloc(sizeof(btSequentialImpulseConstraintSolver), 16);
   btConstraintSolver* constraintSolver = new (mem) btSequentialImpulseConstraintSolver();

   mem = btAlignedAlloc(sizeof(btDiscreteDynamicsWorld), 16);
   btDiscreteDynamicsWorld* world = new (mem) btDiscreteDynamicsWorld(dispatcher, pairCache, constraintSolver, collisionConfiguration);

   world->setDebugDrawer(new DebugDrawer(drawLine));

   (*pworld) = (physics_world_t*)world;
   return 0;
}

void physics_world_delete(struct physics_world_t* world)
{
   btAlignedFree(world);
}

void physics_world_add_rigid_body(struct physics_world_t* world, struct physics_rigid_body_t* body)
{
   ((btDiscreteDynamicsWorld*)world)->addRigidBody((btRigidBody*)body);
}

void physics_world_remove_rigid_body(struct physics_world_t* world, struct physics_rigid_body_t* body)
{
   ((btDiscreteDynamicsWorld*)world)->removeRigidBody((btRigidBody*)body);
}

void physics_world_step(struct physics_world_t* world, float timeStep, int maxSteps, float internalTimeStep)
{
   ((btDiscreteDynamicsWorld*)world)->stepSimulation(timeStep, maxSteps, internalTimeStep);
}

void physics_world_set_gravity(struct physics_world_t* world, const vec3f_t* gravity)
{
   ((btDiscreteDynamicsWorld*)world)->setGravity(vc(gravity));
}

void physics_world_debug_draw(const struct physics_world_t* world)
{
   ((btDiscreteDynamicsWorld*)world)->debugDrawWorld();
}

void physics_rigid_body_delete(struct physics_rigid_body_t* body)
{
   btAlignedFree(body);
}

void physics_rigid_body_apply_central_impulse(struct rigid_body_t* body, const struct vec3f_t* impulse)
{
   ((btRigidBody*)body)->applyCentralImpulse(vc(impulse));
}

void physics_rigid_body_get_transform(struct rigid_body_t* body, mat4f_t* transform)
{
   btTransform& worldTransform = ((btRigidBody*)body)->getWorldTransform();
   worldTransform.getOpenGLMatrix(transform->m);
}

void physics_rigid_body_set_transform(struct rigid_body_t* body, const mat4f_t* transform)
{
   btTransform& worldTransform = ((btRigidBody*)body)->getWorldTransform();
   worldTransform.setFromOpenGLMatrix(transform->m);
}

void physics_rigid_body_set_friction(struct physics_rigid_body_t* body, float friction)
{
   ((btRigidBody*)body)->setFriction(friction);
}

void physics_rigid_body_set_restitution(struct physics_rigid_body_t* body, float restitution)
{
   ((btRigidBody*)body)->setRestitution(restitution);
}

void physics_rigid_body_set_damping(struct physics_rigid_body_t* body, float linear, float angular)
{
   ((btRigidBody*)body)->setDamping(linear, angular);
}

void physics_rigid_body_set_sleeping_thresholds(struct physics_rigid_body_t* body, float linear, float angular)
{
   ((btRigidBody*)body)->setSleepingThresholds(linear, angular);
}

void physics_rigid_body_set_factors(struct physics_rigid_body_t* body, const struct vec3f_t* linear, const struct vec3f_t* angular)
{
   ((btRigidBody*)body)->setLinearFactor(vc(linear));
   ((btRigidBody*)body)->setAngularFactor(vc(angular));
}

int physics_rigid_body_create(struct physics_rigid_body_t** pbody, const struct phys_t* props, struct mesh_t* mesh, motionstate_setter setter, motionstate_getter getter, void* user_data)
{
   if (props->type == phys_t::PHYS_NOCOLLISION)
   {
      return -1;
   }

   long l = 0;

   const struct shape_t* sp = &props->shape;
   physics_shape_t* s = NULL;
   switch (sp->type)
   {
   case shape_t::SHAPE_SPHERE:
      physics_shape_create_sphere(&s, sp->radius);
      break;

   case shape_t::SHAPE_CYLINDER:
      LOGI("CYLINDER: %.2f %.2f", sp->radius, sp->extents.z);
      physics_shape_create_cylinder(&s, sp->radius, sp->extents.z / 2.0f);
      break;

   case shape_t::SHAPE_BOX:
      physics_shape_create_box(&s, sp->extents.x/2.0f, sp->extents.y/2.0f, sp->extents.z/2.0f);
      break;

   case shape_t::SHAPE_CONVEX:
      physics_shape_create_convex(&s, &mesh->vertices[0].point, mesh->nvertices, sizeof(vertex_t));
      break;

   case shape_t::SHAPE_CONCAVE:
   {
      const struct submesh_t* submesh = &mesh->submeshes[0];
      for (l = 0; l < mesh->nsubmeshes; ++l, ++submesh)
      {
         physics_shape_create_concave(&s, &mesh->vertices[0].point, mesh->nvertices, sizeof(vertex_t), &submesh->indices[0], submesh->nindices, 3*sizeof(int));
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

   physics_shape_set_margin(s, sp->margin);

   float mass = (props->type == phys_t::PHYS_RIGID) ? props->mass : 0.0f;
   LOGI("MASS: %.2f INERTIA FACTOR: %.2f", mass, props->inertia_factor);
   LOGI("SLEEPING THRESHOLDS: %.2f %.2f", props->linear_sleeping_threshold, props->angular_sleeping_threshold);
   LOGI("FRICTION: %.2f RESTITUTION: %.2f", props->friction, props->restitution);

   btVector3 localInertia(0, 0, 0);
   if (mass)
   {
      ((btCollisionShape*)s)->calculateLocalInertia(mass, localInertia);
   }

   void* mem = btAlignedAlloc(sizeof(btRigidBody) + sizeof(MotionStateProxy), 16);
   MotionStateProxy* ms = new ((char*)mem + sizeof(btRigidBody)) MotionStateProxy(setter, getter, user_data);
   btRigidBody::btRigidBodyConstructionInfo rbci(mass, ms, (btCollisionShape*)s, localInertia/* * inertia_factor*/);
   btRigidBody* body = new (mem)btRigidBody(rbci);
   body->setUserPointer(user_data);
   ms->mBody = (physics_rigid_body_t*)body;

   (*pbody) = (physics_rigid_body_t*)body;

   physics_rigid_body_set_friction((*pbody), props->friction);
   physics_rigid_body_set_restitution((*pbody), props->restitution);
   physics_rigid_body_set_damping((*pbody), props->linear_damping, props->angular_damping);
   physics_rigid_body_set_sleeping_thresholds((*pbody), props->linear_sleeping_threshold, props->angular_sleeping_threshold);
   physics_rigid_body_set_factors((*pbody), &props->linear_factor, &props->angular_factor);

   return 0;
}

int physics_shape_create_box(struct physics_shape_t** pshape, float x, float y, float z)
{
   void* mem = btAlignedAlloc(sizeof(btBoxShape), 16);
   btCollisionShape* shape = new (mem) btBoxShape(btVector3(x, y, z));
   (*pshape) = (physics_shape_t*)shape;
   return 0;
}

int physics_shape_create_capsule(struct physics_shape_t** pshape, float radius, float height)
{
   const int numSpheres = 2;
   btVector3 positions[numSpheres] = { btVector3(0, height, 0), btVector3(0, -height, 0) };
   btScalar radi[numSpheres] = { radius, radius };

   void* mem = btAlignedAlloc(sizeof(btMultiSphereShape),16);
   btCollisionShape* shape = new (mem) btMultiSphereShape(positions, radi, numSpheres);

   (*pshape) = (physics_shape_t*)shape;
   return 0;
}

int physics_shape_create_cone(struct physics_shape_t** pshape, float radius, float height)
{
   void* mem = btAlignedAlloc(sizeof(btConeShape), 16);
   btCollisionShape* shape = new (mem) btConeShape(radius, height);
   (*pshape) = (physics_shape_t*)shape;
   return 0;
}

int physics_shape_create_cylinder(struct physics_shape_t** pshape, float radius, float height)
{
   void* mem = btAlignedAlloc(sizeof(btCylinderShapeZ), 16);
   btCollisionShape* shape = new (mem) btCylinderShapeZ(btVector3(radius, radius, height));
   (*pshape) = (physics_shape_t*)shape;
   return 0;
}

int physics_shape_create_sphere(struct physics_shape_t** pshape, float radius)
{
   void* mem = btAlignedAlloc(sizeof(btSphereShape), 16);
   btCollisionShape* shape = new (mem) btSphereShape(radius);
   (*pshape) = (physics_shape_t*)shape;
   return 0;
}

int physics_shape_create_concave(struct physics_shape_t** pshape, const struct vec3f_t* vertices, long nvertices, long vertices_stride, const unsigned int* indices, long nindices, long indices_stride)
{
   btIndexedMesh mesh;
   mesh.m_numTriangles = nindices / 3;
   mesh.m_triangleIndexBase = (const unsigned char*)indices;
   mesh.m_triangleIndexStride = indices_stride;
   mesh.m_numVertices = nvertices;
   mesh.m_vertexBase = (const unsigned char*)vertices;
   mesh.m_vertexStride = vertices_stride;
   mesh.m_indexType = PHY_INTEGER;
   mesh.m_vertexType = PHY_FLOAT;

   void* mem = btAlignedAlloc(sizeof(btTriangleIndexVertexArray), 16);
   btTriangleIndexVertexArray* data = new (mem) btTriangleIndexVertexArray();
   data->addIndexedMesh(mesh);

   mem = btAlignedAlloc(sizeof(btBvhTriangleMeshShape), 16);
   btBvhTriangleMeshShape* shape = new (mem) btBvhTriangleMeshShape(data, true, true);

   (*pshape) = (physics_shape_t*)shape;
   return 0;
}

int physics_shape_create_convex(struct physics_shape_t** pshape, const struct vec3f_t* vertices, int nvertices, long stride)
{
   void* mem = btAlignedAlloc(sizeof(btConvexHullShape), 16);
   btConvexHullShape* shape = new (mem) btConvexHullShape((float*)&vertices[0], nvertices, stride);
   (*pshape) = (physics_shape_t*)shape;
   return 0;
}

void physics_shape_delete(struct physics_shape_t* shape)
{
   btAlignedFree(shape);
}

void physics_shape_set_margin(struct physics_shape_t* shape, float margin)
{
   ((btCollisionShape*)shape)->setMargin(margin);
}

