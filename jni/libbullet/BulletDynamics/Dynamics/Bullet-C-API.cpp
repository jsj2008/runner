/*
Bullet Continuous Collision Detection and Physics Library
Copyright (c) 2003-2006 Erwin Coumans  http://continuousphysics.com/Bullet/

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

/*
	Draft high-level generic physics C-API. For low-level access, use the physics SDK native API's.
	Work in progress, functionality will be added on demand.

	If possible, use the richer Bullet C++ API, by including <src/btBulletDynamicsCommon.h>
*/

#include "Bullet-C-Api.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btAlignedAllocator.h"



#include "LinearMath/btVector3.h"
#include "LinearMath/btScalar.h"	
#include "LinearMath/btMatrix3x3.h"
#include "LinearMath/btTransform.h"
#include "BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h"
#include "BulletCollision/CollisionShapes/btTriangleShape.h"

#include "BulletCollision/NarrowPhaseCollision/btGjkPairDetector.h"
#include "BulletCollision/NarrowPhaseCollision/btPointCollector.h"
#include "BulletCollision/NarrowPhaseCollision/btVoronoiSimplexSolver.h"
#include "BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.h"
#include "BulletCollision/NarrowPhaseCollision/btGjkEpaPenetrationDepthSolver.h"
#include "BulletCollision/NarrowPhaseCollision/btGjkEpa2.h"
#include "BulletCollision/CollisionShapes/btMinkowskiSumShape.h"
#include "BulletCollision/NarrowPhaseCollision/btDiscreteCollisionDetectorInterface.h"
#include "BulletCollision/NarrowPhaseCollision/btSimplexSolverInterface.h"
#include "BulletCollision/NarrowPhaseCollision/btMinkowskiPenetrationDepthSolver.h"


/*
	Create and Delete a Physics SDK	
*/

struct	btPhysicsSdk
{

//	btDispatcher*				m_dispatcher;
//	btOverlappingPairCache*		m_pairCache;
//	btConstraintSolver*			m_constraintSolver

	btVector3	m_worldAabbMin;
	btVector3	m_worldAabbMax;


	//todo: version, hardware/optimization settings etc?
	btPhysicsSdk()
		:m_worldAabbMin(-1000,-1000,-1000),
		m_worldAabbMax(1000,1000,1000)
	{

	}

	
};

plPhysicsSdkHandle	plNewBulletSdk()
{
	void* mem = btAlignedAlloc(sizeof(btPhysicsSdk),16);
	return (plPhysicsSdkHandle)new (mem)btPhysicsSdk;
}

void		plDeletePhysicsSdk(plPhysicsSdkHandle	physicsSdk)
{
	btPhysicsSdk* phys = reinterpret_cast<btPhysicsSdk*>(physicsSdk);
	btAlignedFree(phys);	
}


/* Dynamics World */
plDynamicsWorldHandle plCreateDynamicsWorld(plPhysicsSdkHandle physicsSdkHandle)
{
	btPhysicsSdk* physicsSdk = reinterpret_cast<btPhysicsSdk*>(physicsSdkHandle);
	void* mem = btAlignedAlloc(sizeof(btDefaultCollisionConfiguration),16);
	btDefaultCollisionConfiguration* collisionConfiguration = new (mem)btDefaultCollisionConfiguration();
	mem = btAlignedAlloc(sizeof(btCollisionDispatcher),16);
	btDispatcher*				dispatcher = new (mem)btCollisionDispatcher(collisionConfiguration);
	mem = btAlignedAlloc(sizeof(btAxisSweep3),16);
	btBroadphaseInterface*		pairCache = new (mem)btAxisSweep3(physicsSdk->m_worldAabbMin,physicsSdk->m_worldAabbMax);
	mem = btAlignedAlloc(sizeof(btSequentialImpulseConstraintSolver),16);
	btConstraintSolver*			constraintSolver = new(mem) btSequentialImpulseConstraintSolver();

	mem = btAlignedAlloc(sizeof(btDiscreteDynamicsWorld),16);
	return (plDynamicsWorldHandle) new (mem)btDiscreteDynamicsWorld(dispatcher,pairCache,constraintSolver,collisionConfiguration);
}
void           plDeleteDynamicsWorld(plDynamicsWorldHandle world)
{
	//todo: also clean up the other allocations, axisSweep, pairCache,dispatcher,constraintSolver,collisionConfiguration
	btDynamicsWorld* dynamicsWorld = reinterpret_cast< btDynamicsWorld* >(world);
	btAlignedFree(dynamicsWorld);
}

void	plStepSimulation(plDynamicsWorldHandle world,	plReal	timeStep)
{
	btDynamicsWorld* dynamicsWorld = reinterpret_cast< btDynamicsWorld* >(world);
	btAssert(dynamicsWorld);
	dynamicsWorld->stepSimulation(timeStep);
}

void plAddRigidBody(plDynamicsWorldHandle world, plRigidBodyHandle object)
{
	btDynamicsWorld* dynamicsWorld = reinterpret_cast< btDynamicsWorld* >(world);
	btAssert(dynamicsWorld);
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);

	dynamicsWorld->addRigidBody(body);
}

void plRemoveRigidBody(plDynamicsWorldHandle world, plRigidBodyHandle object)
{
	btDynamicsWorld* dynamicsWorld = reinterpret_cast< btDynamicsWorld* >(world);
	btAssert(dynamicsWorld);
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);

	dynamicsWorld->removeRigidBody(body);
}

/* Rigid Body  */

plRigidBodyHandle plCreateRigidBody(	void* user_data,  float mass, plCollisionShapeHandle cshape )
{
	btTransform trans;
	trans.setIdentity();
	btVector3 localInertia(0,0,0);
	btCollisionShape* shape = reinterpret_cast<btCollisionShape*>( cshape);
	btAssert(shape);
	if (mass)
	{
		shape->calculateLocalInertia(mass,localInertia);
	}
	void* mem = btAlignedAlloc(sizeof(btRigidBody),16);
	btRigidBody::btRigidBodyConstructionInfo rbci(mass, 0,shape,localInertia);
	btRigidBody* body = new (mem)btRigidBody(rbci);
	body->setWorldTransform(trans);
	body->setUserPointer(user_data);
	return (plRigidBodyHandle) body;
}

void plDeleteRigidBody(plRigidBodyHandle cbody)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(cbody);
	btAssert(body);
	btAlignedFree( body);
}


/* Collision Shape definition */

plCollisionShapeHandle plNewSphereShape(plReal radius)
{
	void* mem = btAlignedAlloc(sizeof(btSphereShape),16);
	return (plCollisionShapeHandle) new (mem)btSphereShape(radius);
	
}
	
plCollisionShapeHandle plNewBoxShape(plReal x, plReal y, plReal z)
{
	void* mem = btAlignedAlloc(sizeof(btBoxShape),16);
	return (plCollisionShapeHandle) new (mem)btBoxShape(btVector3(x,y,z));
}

plCollisionShapeHandle plNewCapsuleShape(plReal radius, plReal height)
{
	//capsule is convex hull of 2 spheres, so use btMultiSphereShape
	
	const int numSpheres = 2;
	btVector3 positions[numSpheres] = {btVector3(0,height,0),btVector3(0,-height,0)};
	btScalar radi[numSpheres] = {radius,radius};
	void* mem = btAlignedAlloc(sizeof(btMultiSphereShape),16);
	return (plCollisionShapeHandle) new (mem)btMultiSphereShape(positions,radi,numSpheres);
}
plCollisionShapeHandle plNewConeShape(plReal radius, plReal height)
{
	void* mem = btAlignedAlloc(sizeof(btConeShape),16);
	return (plCollisionShapeHandle) new (mem)btConeShape(radius,height);
}

plCollisionShapeHandle plNewCylinderShape(plReal radius, plReal height)
{
	void* mem = btAlignedAlloc(sizeof(btCylinderShape),16);
	return (plCollisionShapeHandle) new (mem)btCylinderShape(btVector3(radius,height,radius));
}

/* Convex Meshes */
plCollisionShapeHandle plNewConvexHullShape()
{
	void* mem = btAlignedAlloc(sizeof(btConvexHullShape),16);
	return (plCollisionShapeHandle) new (mem)btConvexHullShape();
}


/* Concave static triangle meshes */
plMeshInterfaceHandle		   plNewMeshInterface()
{
	return 0;
}

plCollisionShapeHandle plNewCompoundShape()
{
	void* mem = btAlignedAlloc(sizeof(btCompoundShape),16);
	return (plCollisionShapeHandle) new (mem)btCompoundShape();
}

void	plAddChildShape(plCollisionShapeHandle compoundShapeHandle,plCollisionShapeHandle childShapeHandle, plVector3 childPos,plQuaternion childOrn)
{
	btCollisionShape* colShape = reinterpret_cast<btCollisionShape*>(compoundShapeHandle);
	btAssert(colShape->getShapeType() == COMPOUND_SHAPE_PROXYTYPE);
	btCompoundShape* compoundShape = reinterpret_cast<btCompoundShape*>(colShape);
	btCollisionShape* childShape = reinterpret_cast<btCollisionShape*>(childShapeHandle);
	btTransform	localTrans;
	localTrans.setIdentity();
	localTrans.setOrigin(btVector3(childPos[0],childPos[1],childPos[2]));
	localTrans.setRotation(btQuaternion(childOrn[0],childOrn[1],childOrn[2],childOrn[3]));
	compoundShape->addChildShape(localTrans,childShape);
}

void plSetEuler(plReal yaw,plReal pitch,plReal roll, plQuaternion orient)
{
	btQuaternion orn;
	orn.setEuler(yaw,pitch,roll);
	orient[0] = orn.getX();
	orient[1] = orn.getY();
	orient[2] = orn.getZ();
	orient[3] = orn.getW();

}


//	extern  void		plAddTriangle(plMeshInterfaceHandle meshHandle, plVector3 v0,plVector3 v1,plVector3 v2);
//	extern  plCollisionShapeHandle plNewStaticTriangleMeshShape(plMeshInterfaceHandle);


void		plAddVertex(plCollisionShapeHandle cshape, plReal x,plReal y,plReal z)
{
	btCollisionShape* colShape = reinterpret_cast<btCollisionShape*>( cshape);
	(void)colShape;
	btAssert(colShape->getShapeType()==CONVEX_HULL_SHAPE_PROXYTYPE);
	btConvexHullShape* convexHullShape = reinterpret_cast<btConvexHullShape*>( cshape);
	convexHullShape->addPoint(btVector3(x,y,z));

}

void plDeleteShape(plCollisionShapeHandle cshape)
{
	btCollisionShape* shape = reinterpret_cast<btCollisionShape*>( cshape);
	btAssert(shape);
	btAlignedFree(shape);
}
void plSetScaling(plCollisionShapeHandle cshape, plVector3 cscaling)
{
	btCollisionShape* shape = reinterpret_cast<btCollisionShape*>( cshape);
	btAssert(shape);
	btVector3 scaling(cscaling[0],cscaling[1],cscaling[2]);
	shape->setLocalScaling(scaling);	
}



void plSetPosition(plRigidBodyHandle object, const plVector3 position)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
	btVector3 pos(position[0],position[1],position[2]);
	btTransform worldTrans = body->getWorldTransform();
	worldTrans.setOrigin(pos);
	body->setWorldTransform(worldTrans);
}

void plSetOrientation(plRigidBodyHandle object, const plQuaternion orientation)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
	btQuaternion orn(orientation[0],orientation[1],orientation[2],orientation[3]);
	btTransform worldTrans = body->getWorldTransform();
	worldTrans.setRotation(orn);
	body->setWorldTransform(worldTrans);
}

void	plSetOpenGLMatrix(plRigidBodyHandle object, plReal* matrix)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
	btTransform& worldTrans = body->getWorldTransform();
	worldTrans.setFromOpenGLMatrix(matrix);
}

void	plGetOpenGLMatrix(plRigidBodyHandle object, plReal* matrix)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
	body->getWorldTransform().getOpenGLMatrix(matrix);

}

void	plGetPosition(plRigidBodyHandle object,plVector3 position)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
	const btVector3& pos = body->getWorldTransform().getOrigin();
	position[0] = pos.getX();
	position[1] = pos.getY();
	position[2] = pos.getZ();
}

void plGetOrientation(plRigidBodyHandle object,plQuaternion orientation)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
	const btQuaternion& orn = body->getWorldTransform().getRotation();
	orientation[0] = orn.getX();
	orientation[1] = orn.getY();
	orientation[2] = orn.getZ();
	orientation[3] = orn.getW();
}

void plStepSimulationPrecise(plDynamicsWorldHandle world, plReal timeStep, int maxSteps, plReal internalTimeStep)
{
   btDynamicsWorld* dynamicsWorld = reinterpret_cast< btDynamicsWorld* >(world);
   btAssert(dynamicsWorld);
   dynamicsWorld->stepSimulation(timeStep, maxSteps, internalTimeStep);
}

void plApplyCentralImpulse(plRigidBodyHandle object, const plVector3 impulse)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
   btVector3 f (impulse[0], impulse[1], impulse[2]);
   body->applyCentralImpulse(f);
}

void plSetGravity(plDynamicsWorldHandle world, const plVector3 gravity)
{
	btDynamicsWorld* dynamicsWorld = reinterpret_cast< btDynamicsWorld* >(world);
	btAssert(dynamicsWorld);
   btVector3 g (gravity[0], gravity[1], gravity[2]);
	dynamicsWorld->setGravity(g);
}

plCollisionShapeHandle plNewBvhTriangleMeshShape(long nindices, int* indices, long indices_stride, long nvertices, plReal* vertices, long vertices_stride)
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

   void* mem_data = btAlignedAlloc(sizeof(btTriangleIndexVertexArray), 16);
   btTriangleIndexVertexArray* data = new (mem_data)btTriangleIndexVertexArray();
   data->addIndexedMesh(mesh);

   void* mem = btAlignedAlloc(sizeof(btBvhTriangleMeshShape), 16);
   btBvhTriangleMeshShape* shape = new (mem) btBvhTriangleMeshShape(data, true, true);

   return (plCollisionShapeHandle)shape;
}

void plSetFriction(plRigidBodyHandle object, plReal friction)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
   body->setFriction(friction);
}

void plSetRestitution(plRigidBodyHandle object, plReal restitution)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
   body->setRestitution(restitution);
}

void plSetDamping(plRigidBodyHandle object, plReal linear, plReal angular)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
   body->setDamping(linear, angular);
}

void plSetSleepingThresholds(plRigidBodyHandle object, plReal linear, plReal angular)
{
	btRigidBody* body = reinterpret_cast< btRigidBody* >(object);
	btAssert(body);
   body->setSleepingThresholds(linear, angular);
}

//plRigidBodyHandle plRayCast(plDynamicsWorldHandle world, const plVector3 rayStart, const plVector3 rayEnd, plVector3 hitpoint, plVector3 normal);

//	extern  plRigidBodyHandle plObjectCast(plDynamicsWorldHandle world, const plVector3 rayStart, const plVector3 rayEnd, plVector3 hitpoint, plVector3 normal);

double plNearestPoints(float p1[3], float p2[3], float p3[3], float q1[3], float q2[3], float q3[3], float *pa, float *pb, float normal[3])
{
	btVector3 vp(p1[0], p1[1], p1[2]);
	btTriangleShape trishapeA(vp, 
				  btVector3(p2[0], p2[1], p2[2]), 
				  btVector3(p3[0], p3[1], p3[2]));
	trishapeA.setMargin(0.000001f);
	btVector3 vq(q1[0], q1[1], q1[2]);
	btTriangleShape trishapeB(vq, 
				  btVector3(q2[0], q2[1], q2[2]), 
				  btVector3(q3[0], q3[1], q3[2]));
	trishapeB.setMargin(0.000001f);
	
	// btVoronoiSimplexSolver sGjkSimplexSolver;
	// btGjkEpaPenetrationDepthSolver penSolverPtr;	
	
	static btSimplexSolverInterface sGjkSimplexSolver;
	sGjkSimplexSolver.reset();
	
	static btGjkEpaPenetrationDepthSolver Solver0;
	static btMinkowskiPenetrationDepthSolver Solver1;
		
	btConvexPenetrationDepthSolver* Solver = NULL;
	
	Solver = &Solver1;	
		
	btGjkPairDetector convexConvex(&trishapeA ,&trishapeB,&sGjkSimplexSolver,Solver);
	
	convexConvex.m_catchDegeneracies = 1;
	
	// btGjkPairDetector convexConvex(&trishapeA ,&trishapeB,&sGjkSimplexSolver,0);
	
	btPointCollector gjkOutput;
	btGjkPairDetector::ClosestPointInput input;
	
		
	btTransform tr;
	tr.setIdentity();
	
	input.m_transformA = tr;
	input.m_transformB = tr;
	
	convexConvex.getClosestPoints(input, gjkOutput, 0);
	
	
	if (gjkOutput.m_hasResult)
	{
		
		pb[0] = pa[0] = gjkOutput.m_pointInWorld[0];
		pb[1] = pa[1] = gjkOutput.m_pointInWorld[1];
		pb[2] = pa[2] = gjkOutput.m_pointInWorld[2];

		pb[0]+= gjkOutput.m_normalOnBInWorld[0] * gjkOutput.m_distance;
		pb[1]+= gjkOutput.m_normalOnBInWorld[1] * gjkOutput.m_distance;
		pb[2]+= gjkOutput.m_normalOnBInWorld[2] * gjkOutput.m_distance;
		
		normal[0] = gjkOutput.m_normalOnBInWorld[0];
		normal[1] = gjkOutput.m_normalOnBInWorld[1];
		normal[2] = gjkOutput.m_normalOnBInWorld[2];

		return gjkOutput.m_distance;
	}
	return -1.0f;	
}

