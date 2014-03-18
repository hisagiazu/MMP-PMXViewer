#include "bulletphysics.h"
#include "pmx.h"

#include <vector>
#include <string>

using namespace ClosedMMDFormat;

BulletPhysics::BulletPhysics(std::string vertexShaderPath, std::string fragmentShaderPath)
{
	//Use default collision configuration
	collisionConfiguration=new btDefaultCollisionConfiguration();
	
	//Use default collision dispatcher (can use different dispatcher for multi-threading, see Extras/BulletMultiThreaded)
	dispatcher=new btCollisionDispatcher(collisionConfiguration);
	
	//Good general-purpose broadphase. Can also try btAxis3Sweep
	overlappingPairCache = new btDbvtBroadphase();
	
	//Default constraint solver. Can use different solver for multi-threading
	solver=new btSequentialImpulseConstraintSolver();
	
	dynamicsWorld=new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
	
	// gravity scaling
	// official height of miku = 1.58[m]
	// typical height of miku in pmx = 20
	const double gravity = 9.80665*20/1.58;
	dynamicsWorld->setGravity(btVector3(0,-gravity,0));
	
	if(vertexShaderPath!="" && fragmentShaderPath!="")
	{
		debugDrawer=new BulletDebugDrawer(vertexShaderPath, fragmentShaderPath);
		dynamicsWorld->setDebugDrawer(debugDrawer);
		dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	}
	//dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawAabb);
	//dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawConstraints);
	//dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawConstraintLimits);
}
BulletPhysics::~BulletPhysics()
{
	//Remove the rigidbodies from the dynamics world and delete them
	for(int i=dynamicsWorld->getNumCollisionObjects()-1; i>=0; --i)
	{
		btCollisionObject *obj=dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody *body=btRigidBody::upcast(obj);
		
		if(body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}
	
	//delete collision shapes
	for(int i=0; i<collisionShapes.size(); ++i)
	{
		btCollisionShape *shape=collisionShapes[i];
		collisionShapes[i]=0;
		delete shape;
	}
	
	delete dynamicsWorld;
	delete solver;
	
	//delete broadphase
	delete overlappingPairCache;
	
	delete dispatcher;
	delete collisionConfiguration;
	
	collisionShapes.clear();
}

btRigidBody* BulletPhysics::CreateShape(btCollisionShape* shape, const glm::mat4* world,
float mass, float restitution, float friction, float linear_damp, float angular_damp, bool kinematic, unsigned short group, unsigned short mask)
{
	collisionShapes.push_back(shape);
	btVector3 localInertia(0, 0, 0);
	if (mass != 0) shape->calculateLocalInertia(mass, localInertia);
	btDefaultMotionState* motionState = new btDefaultMotionState(ConvertMatrixGLToBT(*world));
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	body->setRestitution(restitution);
	body->setFriction(friction);
	body->setDamping(linear_damp, angular_damp);
	float linearDamp = body->getLinearDamping();
	float angularDamp = body->getAngularDamping();
	if (kinematic) {
		body->setCollisionFlags( body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	}
	body->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(body, group, mask);
	return body;
}

btRigidBody* BulletPhysics::CreateBox(float width, float height, float depth, const glm::mat4* world,
float mass, float restitution, float friction, float linear_damp, float angular_damp, bool kinematic, unsigned short group, unsigned short mask)
{
	btVector3 halfExtents(width/2, height/2, depth/2);
	btCollisionShape* shape = new btBoxShape(halfExtents);
	if (kinematic) mass = 0;
	return CreateShape(shape, world, mass, restitution, friction, linear_damp, angular_damp, kinematic, group, mask);
}

btRigidBody* BulletPhysics::CreateSphere(float radius, const glm::mat4* world,
float mass, float restitution, float friction, float linear_damp, float angular_damp, bool kinematic, unsigned short group, unsigned short mask)
{
	btCollisionShape* shape = new btSphereShape(radius);
	if (kinematic) mass = 0;
	return CreateShape(shape, world, mass, restitution, friction, linear_damp, angular_damp, kinematic, group, mask);
}

btRigidBody* BulletPhysics::CreateCylinder(float radius, float length, const glm::mat4* world,
float mass, float restitution, float friction, float linear_damp, float angular_damp, bool kinematic, unsigned short group, unsigned short mask)
{
	btVector3 halfExtents(radius, radius, length/2);
	btCollisionShape* shape = new btCylinderShape(halfExtents);
	if (kinematic) mass = 0;
	return CreateShape(shape, world, mass, restitution, friction, linear_damp, angular_damp, kinematic, group, mask);
}

btRigidBody* BulletPhysics::CreateCapsule(float radius, float height, const glm::mat4* world,
float mass, float restitution, float friction, float linear_damp, float angular_damp, bool kinematic, unsigned short group, unsigned short mask)
{
	btCollisionShape* shape = new btCapsuleShape(radius, height);
	if (kinematic) mass = 0;
	return CreateShape(shape, world, mass, restitution, friction, linear_damp, angular_damp, kinematic, group, mask);
}


void BulletPhysics::MoveRigidBody(btRigidBody* body, const glm::mat4* world)
{
	btTransform trans = ConvertMatrixGLToBT(*world);
	body->getMotionState()->setWorldTransform(trans);
}

void BulletPhysics::StepSimulation()
{
	dynamicsWorld->stepSimulation(1.0f/60.0f, 10, 1.0f/120.0f);
}

void BulletPhysics::SetDebugMode(int mode)
{
	dynamicsWorld->getDebugDrawer()->setDebugMode(mode);
}
void BulletPhysics::DebugDrawWorld()
{
	dynamicsWorld->debugDrawWorld();
	if(debugDrawer) debugDrawer->render();
}

glm::mat4 BulletPhysics::GetWorld(btRigidBody* body)
{
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	return ConvertMatrixBTToGL(trans);
}





void BulletPhysics::AddPointToPointConstraint(btRigidBody* body, const glm::vec3& pivotDX)
{
	btVector3 pivot = ConvertVectorGLToBT(pivotDX);
	dynamicsWorld->addConstraint(new btPoint2PointConstraint(*body, pivot));
}

void BulletPhysics::AddPointToPointConstraint(btRigidBody* bodyA, btRigidBody* bodyB, const glm::vec3& pivotInADX, const glm::vec3& pivotInBDX)
{
	btVector3 pivotInA = ConvertVectorGLToBT(pivotInADX);
	btVector3 pivotInB = ConvertVectorGLToBT(pivotInBDX);
	dynamicsWorld->addConstraint(new btPoint2PointConstraint(*bodyA, *bodyB, pivotInA, pivotInB));
}

void BulletPhysics::Add6DofSpringConstraint(btRigidBody* bodyA, btRigidBody* bodyB, const glm::mat4& frameInADX, const glm::mat4& frameInBDX,
const glm::vec3& c_p1, const glm::vec3& c_p2, const glm::vec3& c_r1, const glm::vec3& c_r2, std::vector<float> stiffness)
{
	btTransform frameInA = ConvertMatrixGLToBT(frameInADX);
	btTransform frameInB = ConvertMatrixGLToBT(frameInBDX);
	btGeneric6DofSpringConstraint* constraint = new btGeneric6DofSpringConstraint(*bodyA, *bodyB, frameInA, frameInB, true);	// 第五引数の効果は謎。どちらでも同じ様に見える……。
	constraint->setLinearLowerLimit(btVector3(c_p1.x, c_p1.y, c_p1.z));	//型はベクトルだがベクトル量ではないのでZは反転しない。
	constraint->setLinearUpperLimit(btVector3(c_p2.x, c_p2.y, c_p2.z));
	constraint->setAngularLowerLimit(btVector3(c_r1.x, c_r1.y, c_r1.z));
	constraint->setAngularUpperLimit(btVector3(c_r2.x, c_r2.y, c_r2.z));
	for (unsigned int i = 0; i < stiffness.size(); ++i) {
		if (stiffness[i] != 0.0f) {
			constraint->enableSpring(i, true);
			constraint->setStiffness(i, stiffness[i]);
		}
	}
	dynamicsWorld->addConstraint(constraint);	
}

void BulletPhysics::Add6DofSpringConstraint(btRigidBody* bodyB, const glm::mat4& frameInBDX,
const glm::vec3& c_p1, const glm::vec3& c_p2, const glm::vec3& c_r1, const glm::vec3& c_r2, std::vector<float> stiffness)
{
	btTransform frameInB = ConvertMatrixGLToBT(frameInBDX);
	btGeneric6DofSpringConstraint* constraint = new btGeneric6DofSpringConstraint(*bodyB, frameInB, true);	// 第五引数の効果は謎。どちらでも同じ様に見える……。
	constraint->setLinearLowerLimit(btVector3(c_p1.x, c_p1.y, c_p1.z));	//型はベクトルだがベクトル量ではないのでZは反転しない。
	constraint->setLinearUpperLimit(btVector3(c_p2.x, c_p2.y, c_p2.z));
	constraint->setAngularLowerLimit(btVector3(c_r1.x, c_r1.y, c_r1.z));
	constraint->setAngularUpperLimit(btVector3(c_r2.x, c_r2.y, c_r2.z));
	for (unsigned int i = 0; i < stiffness.size(); ++i) {
		if (stiffness[i] != 0.0f) {
			constraint->enableSpring(i, true);
			constraint->setStiffness(i, stiffness[i]);
		}
	}
	dynamicsWorld->addConstraint(constraint);
}










btVector3 BulletPhysics::ConvertVectorGLToBT(const glm::vec3& v)
{
	return btVector3(v.x, v.y, v.z);
}

/// private : Bullet形式とDirectX形式の変換
glm::vec3 BulletPhysics::ConvertVectorBTToGL(const btVector3& v)
{
	return glm::vec3(v.x(), v.y(), v.z());
}

/// private : Bullet形式とDirectX形式の変換
btTransform BulletPhysics::ConvertMatrixGLToBT(const glm::mat4& m)
{	
	btMatrix3x3 basis(	// 鏡像変換＋転置
		m[0][0], m[1][0], m[2][0], 
		m[0][1], m[1][1], m[2][1], 
		m[0][2], m[1][2], m[2][2] );
	return btTransform(basis, btVector3(m[3][0], m[3][1], m[3][2]));
}

/// private : Bullet形式とDirectX形式の変換
glm::mat4 BulletPhysics::ConvertMatrixBTToGL(const btTransform& t)
{
	btVector3 R = t.getBasis().getColumn(0);
	btVector3 U = t.getBasis().getColumn(1);
	btVector3 L = t.getBasis().getColumn(2);
	btVector3 P = t.getOrigin();
	
	glm::mat4 m(	// 鏡像変換＋転置
		R.x(), R.y(), R.z(), 0.f, 
		U.x(), U.y(), U.z(), 0.f,
		L.x(), L.y(), L.z(), 0.f, 
		P.x(), P.y(), P.z(), 1.f );
	return m;
}
