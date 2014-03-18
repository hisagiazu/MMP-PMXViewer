#include <GL/glew.h>
#include <OpenGL/gl.h>

#include "motioncontroller.h"

#include "interpolation.h"
#include "glm_helper.h"

#include <sstream>
#include <iostream>

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/euler_angles.hpp>

//#define MODELDUMP true
#define IK_DUMP false

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace std;
using namespace ClosedMMDFormat;

VMDMotionController::VMDMotionController(PMXInfo &pmxInfo,VMDInfo &vmdInfo,GLuint shaderProgram):pmxInfo(pmxInfo),vmdInfo(vmdInfo)
{	
	//***INIT BONE TRANSFORMATION VARIABLES***
	time=0;
	
	invBindPose=new glm::mat4[pmxInfo.bone_continuing_datasets];
	for(int i=0; i<pmxInfo.bone_continuing_datasets; ++i)
	{
		PMXBone *b = pmxInfo.bones[i];
		invBindPose[i] = glm::translate( -b->position );
	}
	skinMatrix=new glm::mat4[pmxInfo.bone_continuing_datasets]();
	
	Bones_loc=glGetUniformLocation(shaderProgram,"Bones");
	
	
	boneKeyFrames.resize(pmxInfo.bone_continuing_datasets);
	for(unsigned i=0; i<vmdInfo.boneFrames.size(); ++i)
	{
		//cout<<"Searching for match in model for "<<vmdInfo.boneFrames[i].name<<"...";
		for(unsigned j=0; j<pmxInfo.bone_continuing_datasets; ++j)
		{
			//Search for the bone number from the bone name
			if(vmdInfo.boneFrames[i].name == pmxInfo.bones[j]->name)
			{
				//cout<<"Match found";
				boneKeyFrames[j].push_back(vmdInfo.boneFrames[i]);
				continue;
			}
			else
			{
				//cout<<vmdInfo.boneFrames[i].name<<" "<<pmxInfo.bones[j]->name<<endl;
			}
		}
		//cout<<endl;
	}
	
	for(unsigned int i=0; i<pmxInfo.bone_continuing_datasets; ++i)
	{
		boneKeyFrames[i].sort();
		ite_boneKeyFrames.push_back(boneKeyFrames[i].begin());
		boneRot.push_back(glm::quat(1, 0, 0, 0));
		bonePos.push_back(glm::vec3(0, 0, 0));
	}
	
	#ifdef MODELDUMP
	ofstream modeldump("modeldump.txt");
	#endif
	
	//***INIT VERTEX DATA VARIABLES***
	vertexData = (VertexData*)malloc(pmxInfo.vertex_continuing_datasets*sizeof(VertexData)*2);
	for(int i=0; i<pmxInfo.vertex_continuing_datasets; ++i)
	{
		vertexData[i].position.x=pmxInfo.vertices[i]->pos[0];
		vertexData[i].position.y=pmxInfo.vertices[i]->pos[1];
		vertexData[i].position.z=pmxInfo.vertices[i]->pos[2];
		vertexData[i].position.w=1.0;

		vertexData[i].UV.x=pmxInfo.vertices[i]->UV[0];
		vertexData[i].UV.y=pmxInfo.vertices[i]->UV[1];

		vertexData[i].normal.x=pmxInfo.vertices[i]->normal[0];
		vertexData[i].normal.y=pmxInfo.vertices[i]->normal[1];
		vertexData[i].normal.z=pmxInfo.vertices[i]->normal[2];

		vertexData[i].weightFormula=pmxInfo.vertices[i]->weight_transform_formula;

		vertexData[i].boneIndex1=pmxInfo.vertices[i]->boneIndex1;
		vertexData[i].boneIndex2=pmxInfo.vertices[i]->boneIndex2;
		vertexData[i].boneIndex3=pmxInfo.vertices[i]->boneIndex3;
		vertexData[i].boneIndex4=pmxInfo.vertices[i]->boneIndex4;

		vertexData[i].weight1=pmxInfo.vertices[i]->weight1;
		vertexData[i].weight2=pmxInfo.vertices[i]->weight2;
		vertexData[i].weight3=pmxInfo.vertices[i]->weight3;
		vertexData[i].weight4=pmxInfo.vertices[i]->weight4;
                   
		//cout<<vertexData[i].position.x<<" "<<vertexData[i].position.y<<" "<<vertexData[i].position.z<<" "<<vertexData[i].position.w<<endl;
		//cout<<vertexData[i].UV.x<<" "<<vertexData[i].UV.y<<endl;

		/*if(pmxInfo.vertices[i]->weight_transform_formula>2)
		{
			cerr<<"SDEF and QDEF not supported yet"<<endl;
			exit(EXIT_FAILURE);
		}*/
		
		#ifdef MODELDUMP
		modeldump << vertexData[i].str();
		#endif
	}
	glBufferData(GL_ARRAY_BUFFER, pmxInfo.vertex_continuing_datasets*sizeof(VertexData), vertexData, GL_STREAM_DRAW);
	
	morphKeyFrames.resize(pmxInfo.morph_continuing_datasets);
	for(unsigned i=0; i<vmdInfo.morphFrames.size(); ++i)
	{
		for(unsigned j=0; j<pmxInfo.morph_continuing_datasets; ++j)
		{
			//Search for the bone number from the bone name
			if(vmdInfo.morphFrames[i].name == pmxInfo.morphs[j]->name)
			{
				morphKeyFrames[j].push_back(vmdInfo.morphFrames[i]);
				break;
			}
		}
	}
	
	for(unsigned i=0; i<pmxInfo.morph_continuing_datasets; ++i)
	{
		morphKeyFrames[i].sort();
		ite_morphKeyFrames.push_back(morphKeyFrames[i].begin());
		
		vMorphWeights.push_back(0);
	}	
}
VMDMotionController::~VMDMotionController()
{
	free(invBindPose);
	free(skinMatrix);
	//free(vertexData);
}

void VMDMotionController::updateVertexMorphs()
{
	for(unsigned i=0; i<pmxInfo.vertex_continuing_datasets; i++)
	{
		vertexData[i].position.x=pmxInfo.vertices[i]->pos.x;
		vertexData[i].position.y=pmxInfo.vertices[i]->pos.y;
		vertexData[i].position.z=pmxInfo.vertices[i]->pos.z;
		vertexData[i].position.w=1.0f;
	}
	
	for(unsigned i=0; i<pmxInfo.morph_continuing_datasets; ++i)
	{
		PMXMorph *morph=pmxInfo.morphs[i];
		
		if(morph->type==MORPH_TYPE_VERTEX)
		{
			unsigned long t0,t1;
			float val0,val1; //morph values to interpolate between
			glm::vec3 p0,p1;
			
			float ipolValue=0.0f; //interpolated value
			
			if(ite_morphKeyFrames[i] != morphKeyFrames[i].end())
			{
				t0=(*ite_morphKeyFrames[i]).frame*2; //MMD=30fps, this program=60fps, 60/30=2
				val0= (*ite_morphKeyFrames[i]).value;
				
				//cout<<morph->name<<" "<<(*ite_morphKeyFrames[i]).name<<val0<<endl;
			
				if(++ite_morphKeyFrames[i] != morphKeyFrames[i].end())
				{
					t1 = (*ite_morphKeyFrames[i]).frame*2;
					val1 = (*ite_morphKeyFrames[i]).value;
				
					float s = (float)(time - t0)/(float)(t1 - t0);	//Linear Interpolation
					
					
					//ipolValue=val0 + (val1 - val0)*s;
					vMorphWeights[i]=val0 + (val1 - val0)*s;
					
					if(time!=t1) --ite_morphKeyFrames[i];
				}
			}
			
			//cout<<ipolValue<<endl;
			
			for(int j=0; j<morph->morphOffsetNum; ++j)
			{
				PMXVertexMorph *vMorph=(PMXVertexMorph*) morph->offsetData[j];
				glm::vec3 &morphTarget=vMorph->coordinateOffset;
				
				glm::vec3 diffVector=morphTarget;
				
				vertexData[vMorph->vertexIndex].position.x+=diffVector.x * vMorphWeights[i];
				vertexData[vMorph->vertexIndex].position.y+=diffVector.y * vMorphWeights[i];
				vertexData[vMorph->vertexIndex].position.z+=diffVector.z * vMorphWeights[i];
				vertexData[vMorph->vertexIndex].position.w=1.0f;
				//vertexData[vMorph->vertexIndex].position.w=vertexPosition.w;
			}
			
		}
	}
	glBufferData(GL_ARRAY_BUFFER, pmxInfo.vertex_continuing_datasets*sizeof(VertexData), vertexData, GL_DYNAMIC_DRAW);
}

void VMDMotionController::updateBoneMatrix()
{
	for(unsigned i = 0; i < pmxInfo.bone_continuing_datasets; i++)
	{
		skinMatrix[i] = pmxInfo.bones[i]->calculateGlobalMatrix() * invBindPose[i];
	}
	glUniformMatrix4fv(Bones_loc, pmxInfo.bone_continuing_datasets, GL_FALSE, (const GLfloat*)skinMatrix);
}

void VMDMotionController::updateBoneAnimation()
{
	//cout<<"Bezier interpol: "<<bezier(0.25f,0.25f,0.1f,0.25f,1.0f)<<endl;
	//exit(EXIT_FAILURE);
	
	//Root+FKBones
	for(unsigned i = 0; i < pmxInfo.bone_continuing_datasets; i++)
	{
		PMXBone   *b  = pmxInfo.bones[i];
		
		unsigned long t0,t1;
		glm::quat q0,q1;
		glm::vec3 p0,p1;
		
		
		//boneRot[i]=glm::quat(1.0f,0.0f,0.0f,0.0f);
		//bonePos[i]=glm::vec3(0.0f,0.0f,0.0f);
				
		if(ite_boneKeyFrames[i] != boneKeyFrames[i].end())
		{						
			t0=(*ite_boneKeyFrames[i]).frame*2; //MMD=30fps, this program=60fps, 60/30=2
			boneRot[i] = q0 = (*ite_boneKeyFrames[i]).rotation;
			bonePos[i] = p0 = (*ite_boneKeyFrames[i]).translation;
			
			const BezierParameters &bez=(*ite_boneKeyFrames[i]).bezier;
			
			if(++ite_boneKeyFrames[i] != boneKeyFrames[i].end())
			{
				t1 = (*ite_boneKeyFrames[i]).frame*2; //MMD=30fps, this program=60fps, 60/30=2
				q1 = (*ite_boneKeyFrames[i]).rotation;
				p1 = (*ite_boneKeyFrames[i]).translation;
				
				//float s = (float)(time - t0)/(float)(t1 - t0);	//Linear Interpolation
				//cout<<"Bezier: "<<bezier((float)(time - t0)/(float)(t1 - t0), bez.X1.x,bez.X1.y, bez.X2.x,bez.X2.y)<<endl;
				float T=(float)(time - t0)/(float)(t1 - t0);
				float s=0.0f;
				
				//bonePos[i]=p0 + (p1 - p0)*T;
				
				s=bezier(T, bez.X1.x,bez.X1.y, bez.X2.x,bez.X2.y);
				bonePos[i].x=p0.x+ (p1.x-p0.x)*s;
				
				s=bezier(T, bez.Y1.x,bez.Y1.y, bez.Y2.x,bez.Y2.y);
				bonePos[i].y=p0.y+ (p1.y-p0.y)*s;
				
				s=bezier(T, bez.Z1.x,bez.Z1.y, bez.Z2.x,bez.Z2.y);
				bonePos[i].z=p0.z+ (p1.z-p0.z)*s;
				
				s=bezier(T, bez.R1.x,bez.R1.y, bez.R2.x,bez.R2.y);
				boneRot[i]=Slerp(q0,q1,s);
				
				//boneRot[i]=glm::mix(q0,q1,s);
				
				//boneRot[i]=Slerp(q0,q1,T);
				//bonePos[i]=p0 + (p1 - p0)*T;
				
				if(time!=t1) --ite_boneKeyFrames[i];
			}

			if(b->parent)
			{
				b->Local = glm::translate( bonePos[i] + b->position - b->parent->position ) * glm::toMat4(boneRot[i]);
			}
			else
			{
				b->Local = glm::translate( bonePos[i] + b->position ) * glm::toMat4(boneRot[i]);
			}
		}
	}

	updateIK();
}



void VMDMotionController::updateIK()
{
	glm::vec4 effectorPos;
	glm::vec4 targetPos;
	
	glm::vec4 localEffectorPos(0,0,0,0);
	glm::vec4 localTargetPos(0,0,0,0);
	
	glm::vec3 localEffectorDir;
	glm::vec3 localTargetDir;
	
	glm::mat4 tmpMatrix;
	
	glm::mat4 invCoord;
	
	glm::vec3 axis;
	glm::mat4 rotation;

	for(int b=0; b<pmxInfo.bone_continuing_datasets; ++b)
	{
		PMXBone *IKBone=pmxInfo.bones[b];

		if(IKBone->IK) //IKフラグをチェック
		{
			//cout<<IKBone->name<<endl;
			
			glm::mat4 IKBoneMatrix=IKBone->calculateGlobalMatrix();
			
			PMXBone *targetBone=pmxInfo.bones[IKBone->IKTargetBoneIndex];
			
			//cout<<targetBone->name<<endl;
			
			//cout<<IKBone->IKLoopCount<<endl;
			for( int iterations = 0; iterations < IKBone->IKLoopCount; iterations++ )
			{
				for( int attentionIndex=0 ; attentionIndex<IKBone->IKLinkNum; attentionIndex++ )
				{
					PMXIKLink *IKLink=IKBone->IKLinks[attentionIndex];
					PMXBone *linkBone=pmxInfo.bones[IKLink->linkBoneIndex];
					
					//cout<<linkBone->name<<endl;

					tmpMatrix=targetBone->calculateGlobalMatrix();
					effectorPos.x = tmpMatrix[3][0];
					effectorPos.y = tmpMatrix[3][1];
					effectorPos.z = tmpMatrix[3][2];
					effectorPos.w = tmpMatrix[3][3];

					targetPos.x = IKBoneMatrix[3][0];
					targetPos.y = IKBoneMatrix[3][1];
					targetPos.z = IKBoneMatrix[3][2];
					targetPos.w = IKBoneMatrix[3][3];
					
					invCoord=glm::inverse(linkBone->calculateGlobalMatrix());

					localEffectorPos = invCoord * effectorPos;
					localTargetPos = invCoord * targetPos;

					localEffectorDir=glm::normalize(glm::vec3(glm::normalize(localEffectorPos)));
					localTargetDir=glm::normalize(glm::vec3(glm::normalize(localTargetPos)));

					//cout<<localEffectorDir.x<<" "<<localEffectorDir.y<<" "<<localEffectorDir.z<<endl;
					//cout<<localTargetDir.x<<" "<<localTargetDir.y<<" "<<localTargetDir.z<<endl;

					float p = glm::dot(localEffectorDir, localTargetDir);
					//cout<<"P: "<<p<<endl;
					if (p > 1 - 1.0e-6f) continue;
					float angle = acos(p);
					//****注意！！！PMXのこの変数はもうPMDモデルの同じ変数の四倍になってる為、四倍を取る必要はありません。***
					if(angle > IKBone->IKLoopAngleLimit) angle = IKBone->IKLoopAngleLimit;

					axis = -glm::cross(localTargetDir, localEffectorDir);
					rotation = glm::rotate((float)(180.0/M_PI*angle), axis);

					if (IKLink->angleLimit)
					{
						const glm::quat desired_rotation(linkBone->Local * rotation);
						const glm::vec3 desired_euler = glm::radians(glm::eulerAngles(desired_rotation));
						const glm::vec3 clamped_euler = glm::clamp(desired_euler, IKLink->lowerLimit, IKLink->upperLimit);
						const glm::quat clamped_rotation(clamped_euler);
						const glm::mat4 translation = glm::translate(linkBone->Local[3][0], linkBone->Local[3][1], linkBone->Local[3][2]);
						linkBone->Local = translation * glm::toMat4(clamped_rotation);
					}
					else
					{
						linkBone->Local *= rotation;
					}
				}
				const float errToleranceSq = 0.00001f;
				if(glm::length2(localEffectorPos - localTargetPos) < errToleranceSq)
				{
					break;
				}
			}
		}
	}

	// dump rotation of IK link bone from parent
	if (IK_DUMP)
	{
		for(int b=0; b<pmxInfo.bone_continuing_datasets; ++b)
		{
			const PMXBone &bone=*pmxInfo.bones[b];
			if(!bone.IK) continue;
			for(int attentionIndex=0 ; attentionIndex<bone.IKLinkNum; attentionIndex++)
			{
				const PMXBone &linkBone=*pmxInfo.bones[bone.IKLinks[attentionIndex]->linkBoneIndex];
				if (time == 0)
				{
					cerr<<linkBone.name<<"[X], "<<linkBone.name<<"[Y], "<<linkBone.name<<"[Z]"<<", ";
				}
				else
				{
					const glm::quat q(linkBone.Local);
					const glm::vec3 euler = glm::degrees(toEulerAnglesRadians(q));
					cerr<<euler<<", ";
				}
			}
		}
		cerr<<endl;
	}
}

bool VMDMotionController::advanceTime()
{
	++time;

	// if all boneKeyFrames are finished, return true
	for(unsigned i = 0; i < pmxInfo.bone_continuing_datasets; i++)
	{
		if (ite_boneKeyFrames[i] != boneKeyFrames[i].end())
		{
			return false;
		}
	}
	return true;
}
