#include "mmdphysics.h"
#include "pmx.h"
#include "glm_helper.h"

#include <iostream>

using namespace std;
using namespace ClosedMMDFormat;

MMDPhysics::MMDPhysics(PMXInfo &pmxInfo, VMDMotionController *motionController, BulletPhysics* bulletPhysics):pmxInfo(pmxInfo),motionController(motionController)
{
	this->bulletPhysics=bulletPhysics;
	//this->joint_mesh = 0;
	
	createRigidBody();
	createJoints();
	
	//createTestRigidBody
	/*glm::mat4 world;
	int i=-1;
	glm::vec3 position(0,5.0,0);
	glm::vec3 rotation(0,0.0,0);
	
	world = createRigidMatrix(position, rotation, i);
	
	float radius=5.f;
	float mass=5.f;
	float elasticity=5.f;
	float friction=5.f;
	float movementDecay=5.f;
	float rotationDecay=5.f;
	bool physicsOperation=true;
	int group=0;
	bool noCollisionGroupFlag=false;
			
	this->rigidBodies.push_back(bulletPhysics->CreateSphere(
radius, &world, mass, elasticity, friction, movementDecay,
rotationDecay, physicsOperation == 0, 1 << group, noCollisionGroupFlag));*/

	//createTestJoint
}

glm::mat4 MMDPhysics::createRigidMatrix(const glm::vec3 &pos, const glm::vec3 &rot)
{
	return glm::translate(pos)*glm::toMat4(fromEulerAnglesRadians(rot));
}

vector<glm::vec3> createBox(float width, float height, float depth)
{
	vector<glm::vec3> mesh;
	
	mesh.push_back(glm::vec3(-width,height,-depth));
	mesh.push_back(glm::vec3(-width,-height,-depth));
	mesh.push_back(glm::vec3(width,-height,-depth));
	
	mesh.push_back(glm::vec3(width,-height,-depth));
	mesh.push_back(glm::vec3(width,height,-depth));
	mesh.push_back(glm::vec3(-width,height,-depth));
	
	mesh.push_back(glm::vec3(-width,height,depth));
	mesh.push_back(glm::vec3(-width,-height,depth));
	mesh.push_back(glm::vec3(width,-height,depth));
	
	mesh.push_back(glm::vec3(width,-height,depth));
	mesh.push_back(glm::vec3(width,height,depth));
	mesh.push_back(glm::vec3(-width,height,depth));
	
	
	mesh.push_back(glm::vec3(width,height,-depth));
	mesh.push_back(glm::vec3(width,height,depth));
	mesh.push_back(glm::vec3(width,-height,depth));
	
	mesh.push_back(glm::vec3(width,-height,depth));
	mesh.push_back(glm::vec3(width,-height,-depth));
	mesh.push_back(glm::vec3(width,height,-depth));
	
	mesh.push_back(glm::vec3(-width,height,-depth));
	mesh.push_back(glm::vec3(-width,height,depth));
	mesh.push_back(glm::vec3(-width,-height,depth));
	
	mesh.push_back(glm::vec3(-width,-height,depth));
	mesh.push_back(glm::vec3(-width,-height,-depth));
	mesh.push_back(glm::vec3(-width,height,-depth));
	
	
	mesh.push_back(glm::vec3(width,height,-depth));
	mesh.push_back(glm::vec3(width,height,depth));
	mesh.push_back(glm::vec3(-width,height,depth));
	
	mesh.push_back(glm::vec3(-width,height,depth));
	mesh.push_back(glm::vec3(-width,height,-depth));
	mesh.push_back(glm::vec3(width,height,-depth));
	
	mesh.push_back(glm::vec3(width,-height,-depth));
	mesh.push_back(glm::vec3(width,-height,depth));
	mesh.push_back(glm::vec3(-width,-height,depth));
	
	mesh.push_back(glm::vec3(-width,-height,depth));
	mesh.push_back(glm::vec3(-width,-height,-depth));
	mesh.push_back(glm::vec3(width,-height,-depth));
	
	/*mesh.push_back(glm::vec3(-width,height,depth));
	mesh.push_back(glm::vec3(-width,-height,depth));
	mesh.push_back(glm::vec3(width,height,depth));
	mesh.push_back(glm::vec3(width,-height,depth));*/
	
	return mesh;
}

void MMDPhysics::createRigidBody()
{
	const std::vector<PMXRigidBody*> &rigidBodies=pmxInfo.rigidBodies;

	for(int i=0; i<pmxInfo.rigid_body_continuing_datasets; ++i)
	{
		glm::mat4 world=createRigidMatrix(rigidBodies[i]->position, rigidBodies[i]->rotation);
		rigidBodies[i]->Init=world;
		rigidBodies[i]->Offset=glm::inverse(world);

		if (rigidBodies[i]->shape == RIGID_SHAPE_SPHERE) //球
		{
			float radius = rigidBodies[i]->size.x;
			
			this->rigidBodies.push_back(bulletPhysics->CreateSphere(
radius, &world, rigidBodies[i]->mass, rigidBodies[i]->elasticity, rigidBodies[i]->friction, rigidBodies[i]->movementDecay,
rigidBodies[i]->rotationDecay, rigidBodies[i]->physicsOperation == 0, 1 << rigidBodies[i]->group, rigidBodies[i]->noCollisionGroupFlag));
				
			const unsigned int slices = 10, stacks = 5;
			//createSphere(radius,slices,stacks); //D3DXCreateSphere(pDevice, radius, slices, stacks, &mesh, 0);
			//rigidbody_mesh.push_back(mesh);
		}
		else if (rigidBodies[i]->shape == RIGID_SHAPE_CUBE) //箱
		{
			float width = rigidBodies[i]->size.x, height = rigidBodies[i]->size.y, depth = rigidBodies[i]->size.z;
			
			this->rigidBodies.push_back(bulletPhysics->CreateBox(
width, height, depth, &world, rigidBodies[i]->mass, rigidBodies[i]->elasticity, rigidBodies[i]->friction, rigidBodies[i]->movementDecay,
rigidBodies[i]->rotationDecay, rigidBodies[i]->physicsOperation == 0, 1 << rigidBodies[i]->group, rigidBodies[i]->noCollisionGroupFlag));

			//D3DXCreateBox(pDevice, width, height, depth, &mesh, 0);
			//vector<glm::vec3> mesh=createBox(width,height,depth);
			//rigidMeshes.push_back(mesh);
			//rigidMeshIndices.push_back(i);
		}
		else if(rigidBodies[i]->shape == RIGID_SHAPE_CAPSULE) //カプセル
		{
			float radius = rigidBodies[i]->size.x, height = rigidBodies[i]->size.y;
			
			this->rigidBodies.push_back(bulletPhysics->CreateCapsule(
radius, height, &world, rigidBodies[i]->mass, rigidBodies[i]->elasticity, rigidBodies[i]->friction, rigidBodies[i]->movementDecay,
rigidBodies[i]->rotationDecay, rigidBodies[i]->physicsOperation == 0, 1 << rigidBodies[i]->group, rigidBodies[i]->noCollisionGroupFlag));
				
			const unsigned int slices = 10, stacks = 5;
			//D3DXCreateCylinder(pDevice, radius, radius, height, slices, stacks, &mesh, 0);	// カプセル形状の代わりに円柱で表示
			//createCylinder(radius,height,slices,stacks);
			//rigidbody_mesh.push_back(mesh);
		}
	}
}

void MMDPhysics::createJoints()
{
	for (unsigned int i = 0; i<pmxInfo.joint_continuing_datasets; ++i)
	{
		PMXJoint *joint=pmxInfo.joints[i];
		
		glm::vec3 c_p1=(joint->movementLowerLimit);
		glm::vec3 c_p2(joint->movementUpperLimit);
		glm::vec3 c_r1(joint->rotationLowerLimit);
		glm::vec3 c_r2(joint->rotationUpperLimit);
		glm::vec3 s_p(joint->springMovementConstant);
		glm::vec3 s_r(joint->springRotationConstant);
		
		vector<float> stiffness;
		
		for (int j =  0; j < 3; ++j) stiffness.push_back(joint->springMovementConstant[j]);
		for (int j =  0; j < 3; ++j) stiffness.push_back(joint->springRotationConstant[j]);
		
		glm::mat4 world = createRigidMatrix(joint->position, joint->rotation);	// ジョイントの行列（モデルローカル座標系）
		
		btRigidBody *rigidbody_a,*rigidbody_b;
		glm::mat4 a,b,frameInA,frameInB;
		
		rigidbody_a = rigidBodies[joint->relatedRigidBodyIndexA];
		a = bulletPhysics->GetWorld(rigidbody_a);	// 剛体の行列（モデルローカル座標系）
		a=glm::inverse(a);
		frameInA = a*world;
		
		if(joint->relatedRigidBodyIndexB!=-1)
		{
			rigidbody_b = rigidBodies[joint->relatedRigidBodyIndexB];
			b = bulletPhysics->GetWorld(rigidbody_b);
			b=glm::inverse(b);
			frameInB = b*world;	// ジョイントの行列（剛体ローカル座標系）
		}

		if(joint->relatedRigidBodyIndexB!=-1)
		{
			bulletPhysics->Add6DofSpringConstraint(rigidbody_a, rigidbody_b, frameInA, frameInB, c_p1, c_p2, c_r1, c_r2, stiffness);
			joint->Local=frameInA;
		}
		else
		{
			bulletPhysics->Add6DofSpringConstraint(rigidbody_a, frameInA, c_p1, c_p2, c_r1, c_r2, stiffness);
			joint->Local=frameInA;
		}
	}
	//const float length = 0.3f;
	//D3DXCreateBox(pDevice, length, length, length, &joint_mesh, 0);
}


void MMDPhysics::updateBones(bool physicsEnabled)
{
	// ボーン追従タイプの剛体にボーン行列を設定
	for (unsigned int i_rb = 0; i_rb<pmxInfo.rigidBodies.size(); ++i_rb)
	{
		// ボーン追従タイプ以外の剛体は飛ばす
		if (pmxInfo.rigidBodies[i_rb]->relatedBoneIndex == -1 ||
			pmxInfo.rigidBodies[i_rb]->physicsOperation != RIGID_TYPE_FOLLOWER)
		{
			continue;
		}

		const unsigned int i_bone = pmxInfo.rigidBodies[i_rb]->relatedBoneIndex;
		PMXBone* bone = pmxInfo.bones[i_bone];
		// ボーンの移動量を剛体の初期姿勢に適用したものが剛体の現在の姿勢
		const glm::mat4 rigidMat = pmxInfo.bones[i_bone]->calculateGlobalMatrix()*motionController->invBindPose[i_bone]*pmxInfo.rigidBodies[i_rb]->Init;
		bulletPhysics->MoveRigidBody(rigidBodies[i_rb], &rigidMat);
	}

	if(physicsEnabled) bulletPhysics->StepSimulation();

	// グローバル座標系の剛体位置姿勢をPMXBoneの親リンクからの相対位置姿勢に変換する．
	// PMXBoneの親子関係を考慮していないので収束ループが必要となりあまり良いコードではない．
	// Appearance Miku & TDA Mikuでテストし収束ループは２回とした
	for (int i_it = 0; i_it < 2; ++i_it)
	{
		for (unsigned int i_rb = 0; i_rb < rigidBodies.size(); ++i_rb)
		{
			// ボーン追従タイプの剛体は飛ばす
			if (pmxInfo.rigidBodies[i_rb]->relatedBoneIndex == -1 ||
				pmxInfo.rigidBodies[i_rb]->physicsOperation == RIGID_TYPE_FOLLOWER)
			{
				continue;
			}

			const unsigned int i_bone = pmxInfo.rigidBodies[i_rb]->relatedBoneIndex;
			PMXBone* bone = pmxInfo.bones[i_bone];
			glm::mat4 parent_global;
			if (bone->parent)
			{
				parent_global = bone->parent->calculateGlobalMatrix();
			}
			const glm::mat4 rb_global = bulletPhysics->GetWorld(rigidBodies[i_rb]) * pmxInfo.rigidBodies[i_rb]->Offset;
			const glm::mat4 rb_local = glm::inverse(parent_global) * rb_global * glm::inverse(motionController->invBindPose[i_bone]);

			if (pmxInfo.rigidBodies[i_rb]->physicsOperation == RIGID_TYPE_PHYSICS)
			{
				bone->Local = rb_local;
			}
			else if (pmxInfo.rigidBodies[i_rb]->physicsOperation == RIGID_TYPE_PHYSICS_ROT_ONLY)
			{
				const glm::quat rotation(rb_local);
				bone->Local = glm::translate(bone->Local[3][0], bone->Local[3][1], bone->Local[3][2]) * glm::toMat4(rotation);
			}
		}
	}
}
