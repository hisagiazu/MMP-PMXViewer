#include "vmd.h"

#include <fstream>
#include <iostream>
#include <bitset>
#include <sstream>

#include "texthandle.h"

using namespace std;

namespace ClosedMMDFormat
{

	VMDInfo &readVMD(string filename)
	{
		VMDInfo *vInfo = new VMDInfo();
		VMDInfo &vmdInfo = *vInfo;
		
		ifstream miku(filename.c_str(), ios::in | ios::binary);
		if (!miku) { cerr<<"ERROR: VMD file could not be found: "<<filename<<endl; }
		
		//***Extract Header Info***
		char headerStr[30];
		miku.read((char*)&headerStr, 30);
		
		char modelName[20];
		miku.read((char*)&modelName, 20);
		vmdInfo.modelName=sjisToUTF8(modelName);
		
		cout<<headerStr<<endl;
		//cout<<vmdInfo.modelName<<endl;
		
		//***Extract Bone Info***
		unsigned boneCount=0;
		miku.read((char*)&boneCount, 4);
		vmdInfo.boneFrames.resize(boneCount);
		
		//cout<<"Bone Count: "<<boneCount<<endl;
		
		cout<<"Loading bone frames...";
		for(int i=0; i < vmdInfo.boneFrames.size(); ++i)
		{
			VMDBoneFrame *f = &vmdInfo.boneFrames[i];
			
			char name[15];
			miku.read((char*)name, 15);
			f->name=sjisToUTF8(name);
			
			miku.read((char*)&f->frame, 4);
			
			miku.read((char*)&f->translation.x, 4);
			miku.read((char*)&f->translation.y, 4);
			miku.read((char*)&f->translation.z, 4);
			//f->translation.x=-f->translation.x;
			f->translation.z = -f->translation.z;
			
			//cout<<f->position.x<<" "<<f->position.y<<" "<<f->position.z<<endl;
			
			miku.read((char*)&f->rotation.x, 4);
			miku.read((char*)&f->rotation.y, 4);
			miku.read((char*)&f->rotation.z, 4);
			miku.read((char*)&f->rotation.w, 4);
			//f->rotation.x=-f->rotation.x;
			//f->rotation.y=-f->rotation.y;
			//f->rotation.z=-f->rotation.z;
			//f->rotation.w=-f->rotation.w;
			glm::mat3 rotMatrix=glm::toMat3(f->rotation);
			rotMatrix[2][0] = -rotMatrix[2][0];
			rotMatrix[2][1] = -rotMatrix[2][1];
			rotMatrix[0][2] = -rotMatrix[0][2];
			rotMatrix[1][2] = -rotMatrix[1][2];
			f->rotation=glm::toQuat(rotMatrix);
			
			uint8_t bezier[64];
			miku.read((char*)bezier,64);
			
			//Cubic bezier variables
			//line 1
			f->bezier.X1.x = bezier[0];
			f->bezier.Y1.x = bezier[1];
			f->bezier.Z1.x = bezier[2];
			f->bezier.R1.x = bezier[3];
			
			f->bezier.X1.y = bezier[4];
			f->bezier.Y1.y = bezier[5];
			f->bezier.Z1.y = bezier[6];
			f->bezier.R1.y = bezier[7];
			
			//line 2
			f->bezier.X2.x = bezier[8];
			f->bezier.Y2.x = bezier[9];
			f->bezier.Z2.x = bezier[10];
			f->bezier.R2.x = bezier[11];
			
			f->bezier.X2.y = bezier[12];
			f->bezier.Y2.y = bezier[13];
			f->bezier.Z2.y = bezier[14];
			f->bezier.R2.y = bezier[15];
			
			//cout<<f->bezier.Y1.x<<" "<<(int)bezier[1]<<" "<<(int)bezier[16]<<endl;
			//exit(EXIT_FAILURE);
			
			/*cout<<f->bezier.X1.x<<" "<<f->bezier.X1.y<<" "<<f->bezier.Y1.x<<" "<<f->bezier.Y1.y<<" "<<f->bezier.Z1.x<<" "<<f->bezier.Z1.y<<" "<<f->bezier.R1.x<<" "<<f->bezier.R1.y<<endl
			<<f->bezier.X2.x<<" "<<f->bezier.X2.y<<" "<<f->bezier.Y2.x<<" "<<f->bezier.Y2.y<<" "<<f->bezier.Z2.x<<" "<<f->bezier.Z2.y<<" "<<f->bezier.R2.x<<" "<<f->bezier.R2.y<<endl<<endl;*/
			
			
			/*cout<<"Name: "<<f->name<<endl;
			cout<<"Frame Number: "<<f->frame<<endl<<endl;
			
			cout<<"Position: "<<f->position.x<<" "<<f->position.y<<" "<<f->position.z<<endl;
			cout<<"Quaternion: "<<f->quaternion.x<<" "<<f->quaternion.y<<" "<<f->quaternion.z<<" "<<f->quaternion.w<<endl;*/
		}
		cout<<"done."<<endl;
		
		//***Extract Morph Info***
		unsigned morphCount=0;
		miku.read((char*)&morphCount, 4);
		vmdInfo.morphFrames.resize(morphCount);
		
		//cout<<"Morph Count: "<<vmdInfo.morphCount<<endl;
		
		cout<<"Loading morph frames...";
		for(int i=0; i < vmdInfo.morphFrames.size(); ++i)
		{
			VMDMorphFrame *f = &vmdInfo.morphFrames[i];
			
			char name[15];
			miku.read((char*)&name, 15);
			f->name = sjisToUTF8(name);
			
			miku.read((char*)&f->frame, 4);
			miku.read((char*)&f->value, 4);

			/*cout<<"Name: "<<f->name<<endl;
			cout<<"Frame Number: "<<f->frame<<endl;
			cout<<"Value: "<<f->value<<endl<<endl;*/
		}
		cout<<"done."<<endl;
		
		///NOTHING PAST THIS POINT TESTED (yet)
		
		//***Extract Camera Info***
		unsigned int cameraCount=0;
		miku.read((char*)&cameraCount, 4);
		vmdInfo.cameraFrames.resize(cameraCount);
		//cout<<"Camera Count: "<<vmdInfo.cameraCount<<endl;
		
		cout<<"Loading camera frames...";
		for(int i=0; i < vmdInfo.cameraFrames.size(); ++i)
		{
			VMDCameraFrame *f = &vmdInfo.cameraFrames[i];
			
			miku.read((char*)&f->frame, 4);
			
			miku.read((char*)&f->position.x, 4);
			miku.read((char*)&f->position.y, 4);
			miku.read((char*)&f->position.z, 4);
			f->position.z = -f->position.z;
			
			miku.read((char*)&f->rotation.x, 4);
			miku.read((char*)&f->rotation.y, 4);
			miku.read((char*)&f->rotation.z, 4);
			
			//WARNING!!!! IS f->rotation a set of Euler Angles? (Probably yes)
			//If so, we should be performing Quaternion Mirroring like in pmx.cpp:506
			
			//!\todo Read Camera Frame Bezier Parameters correctly
			char bezier[24];
			miku.read((char*)&bezier, 24);
		}
		cout<<"done."<<endl;	
		
		
		//***Extract Light Info***
		unsigned lightCount=0;
		miku.read((char*)&lightCount, 4);
		vmdInfo.lightFrames.resize(lightCount);
		//cout<<"Light Count: "<<lightCount<<endl;
		
		cout<<"Loading light frames...";
		for(int i=0; i < vmdInfo.lightFrames.size(); ++i)
		{
			VMDLightFrame *f = &vmdInfo.lightFrames[i];
			
			miku.read((char*)&f->frame, 4);
			
			miku.read((char*)&f->color.r, 4);
			miku.read((char*)&f->color.g, 4);
			miku.read((char*)&f->color.b, 4);

			miku.read((char*)&f->position.x, 4);
			miku.read((char*)&f->position.y, 4);
			miku.read((char*)&f->position.z, 4);
			f->position.z = -f->position.z;
		}
		cout<<"done."<<endl;
	
		//***Extract Self Shadow Info***
		unsigned selfShadowCount=0;
		miku.read((char*)&selfShadowCount, 4);
		vmdInfo.selfShadowFrames.resize(selfShadowCount);
		//cout<<"SelfShadow Count: "<<selfShadowCount<<endl;
		
		cout<<"Loading SelfShadow frames...";
		for(int i=0; i < vmdInfo.selfShadowFrames.size(); ++i)
		{



			VMDSelfShadowFrame *f = &vmdInfo.selfShadowFrames[i];
			
			miku.read((char*)&f->frame,    4);
			miku.read((char*)&f->type,     1);
			miku.read((char*)&f->distance, 4);
		}
		cout<<"done."<<endl;
		
		//***Extract Show IK Frame Info***
		unsigned showIKCount=0;
		miku.read((char*)&showIKCount, 4);
		vmdInfo.showIKFrames.resize(showIKCount);
		//cout<<"ShowIK Count: "<<showIKCount<<endl;
		
		cout<<"Loading ShowIK frames...";
		for(int i=0; i < vmdInfo.showIKFrames.size(); ++i)
		{
			VMDShowIKFrame *f = &vmdInfo.showIKFrames[i];
			
			unsigned IKCount=0;
			miku.read((char*)&f->frame,    4);
			miku.read((char*)&f->show,     1);
			miku.read((char*)&IKCount,  4);
			f->IKList.resize(IKCount);
			//cout<<"IK Count: "<<IKCount<<endl;
			
			for(int i=0; i < f->IKList.size(); ++i)
			{
				VMDIKInfo *info = &f->IKList[i];
				char name[20];
				miku.read((char*)&name, 20);
				info->name = sjisToUTF8(name);
				miku.read((char*)&info->isOn, 1);
			}
		}
		cout<<"done."<<endl;

		cout<<endl;
		
		return vmdInfo;
		
		//exit(EXIT_SUCCESS);
	}
	
	void writeVMD(VMDInfo &vmdInfo, string filename)
	{
		ofstream miku(filename.c_str(), ios::out | ios::binary);
		if (!miku) { cerr<<"ERROR: VMD file could not be found: "<<filename<<endl; }
		
		static const char headerStr[30]="Vocaloid Motion Data 0002\0\0\0\0"; //The last '\0' is implied
		string modelName=UTF8ToSJIS(vmdInfo.modelName.c_str()); //TODO: Convert UTF8 string back to SHIFT_JIS!!!

		miku.write(headerStr,30);
		miku.write(modelName.c_str(),20);
		
		
		unsigned boneCount=vmdInfo.boneFrames.size();
		miku.write((char*)&boneCount,4);
		
		for(unsigned i=0; i<vmdInfo.boneFrames.size(); ++i)
		{
			VMDBoneFrame *f = &vmdInfo.boneFrames[i];
			
			string name=UTF8ToSJIS(f->name.c_str());
			
			glm::vec3 translation=f->translation;
			translation.z=-translation.z; //Flip Z-axis back to DirectX (left-handed) coordinate form
			
			glm::quat rotation;
			
			//Flip Quaternion back to DirectX (left-handed) coordinate form
			glm::mat3 rotMatrix=glm::toMat3(f->rotation);
			rotMatrix[2][0] = -rotMatrix[2][0];
			rotMatrix[2][1] = -rotMatrix[2][1];
			rotMatrix[0][2] = -rotMatrix[0][2];
			rotMatrix[1][2] = -rotMatrix[1][2];
			rotation=glm::toQuat(rotMatrix);
			
			uint8_t bezier[64];
			//VMD stores bezier parameters in a really weird/meaningless way. For details:
			//http://harigane.at.webry.info/201103/article_1.html
			bezier[ 0] = f->bezier.X1.x;
			bezier[ 1] = bezier[16] = f->bezier.Y1.x;
			bezier[ 2] = bezier[17] = bezier[32] = f->bezier.Z1.x;
			bezier[ 3] = bezier[18] = bezier[33] = bezier[48] = f->bezier.R1.x;
			
			bezier[ 4] = bezier[19] = bezier[34] = bezier[49] = f->bezier.X1.y;
			bezier[ 5] = bezier[20] = bezier[35] = bezier[50] = f->bezier.Y1.y;
			bezier[ 6] = bezier[21] = bezier[36] = bezier[51] = f->bezier.Z1.y;
			bezier[ 7] = bezier[22] = bezier[37] = bezier[52] = f->bezier.R1.y;
			
			//line 2
			bezier[ 8] = bezier[23] = bezier[38] = bezier[53] = f->bezier.X2.x;
			bezier[ 9] = bezier[24] = bezier[39] = bezier[54] = f->bezier.Y2.x;
			bezier[10] = bezier[25] = bezier[40] = bezier[55] = f->bezier.Z2.x;
			bezier[11] = bezier[26] = bezier[41] = bezier[56] = f->bezier.R2.x;
			
			bezier[12] = bezier[27] = bezier[42] = bezier[57] = f->bezier.X2.y;
			bezier[13] = bezier[28] = bezier[43] = bezier[58] = f->bezier.Y2.y;
			bezier[14] = bezier[29] = bezier[44] = bezier[59] = f->bezier.Z2.y;
			bezier[15] = bezier[30] = bezier[45] = bezier[60] = f->bezier.R2.y;
			
						 bezier[31] = bezier[46] = bezier[61] = 01;
									  bezier[47] = bezier[62] = bezier[63] = 00;
			
			
			miku.write(name.c_str(),15);
			
			miku.write((char*)&f->frame,4);
			miku.write((char*)&translation.x,4);
			miku.write((char*)&translation.y,4);
			miku.write((char*)&translation.z,4);
			
			//Write each axis individually to ensure correct order
			miku.write((char*)&rotation.x, 4);
			miku.write((char*)&rotation.y, 4);
			miku.write((char*)&rotation.z, 4);
			miku.write((char*)&rotation.w, 4);
			
			miku.write((char*)&bezier,64);
		}
		
		unsigned morphCount=vmdInfo.morphFrames.size();
		miku.write((char*)&morphCount,4);
		
		for(unsigned i=0; i<vmdInfo.morphFrames.size(); ++i)
		{
			VMDMorphFrame *f = &vmdInfo.morphFrames[i];
			
			string name=UTF8ToSJIS(f->name.c_str()).c_str();
			
			miku.write(name.c_str(),15);
			miku.write((char*)&f->frame, 4);
			miku.write((char*)&f->value, 4);
		}
		
		unsigned cameraCount=vmdInfo.cameraFrames.size();
		miku.write((char*)&cameraCount,4);
		
		for(unsigned i=0; i<vmdInfo.cameraFrames.size(); ++i)
		{
			VMDCameraFrame *f = &vmdInfo.cameraFrames[i];
			
			glm::vec3 position=f->position;
			position.z=-position.z; //Flip Z-axis back to DirectX (left-handed) coordinate form
			
			//WARNING!!!! IS f->rotation a set of Euler Angles? (Probably yes)
			//If so, we should be performing Quaternion Mirroring like in pmx.cpp:506
			
			char bezier[24]; //!\todo  Write Camera Frame Bezier Parameters correctly
			
			miku.write((char*)&position.x,4);
			miku.write((char*)&position.y,4);
			miku.write((char*)&position.z,4);
			
			miku.write((char*)&f->rotation.x,4);
			miku.write((char*)&f->rotation.y,4);
			miku.write((char*)&f->rotation.z,4);
			
			miku.write(bezier,24); //!\todo  Write Camera Frame Bezier Parameters correctly
		}
		
		unsigned lightCount=vmdInfo.lightFrames.size();
		miku.write((char*)&lightCount,4);
		
		for(unsigned i=0; i<vmdInfo.lightFrames.size(); ++i)
		{
			VMDLightFrame *f = &vmdInfo.lightFrames[i];
			
			glm::vec3 position=f->position;
			position.z=-position.z;
			
			miku.write((char*)&f->frame, 4);
			
			miku.write((char*)&f->color.r, 4);
			miku.write((char*)&f->color.g, 4);
			miku.write((char*)&f->color.b, 4);

			miku.write((char*)&position.x, 4);
			miku.write((char*)&position.y, 4);
			miku.write((char*)&position.z, 4);
		}
		
		//***Extract Self Shadow Info***
		unsigned selfShadowCount=vmdInfo.selfShadowFrames.size();
		miku.write((char*)&selfShadowCount,4);

		for(int i=0; i<vmdInfo.selfShadowFrames.size(); ++i)
		{
			VMDSelfShadowFrame *f = &vmdInfo.selfShadowFrames[i];
			
			miku.write((char*)&f->frame, 4);
			miku.write((char*)&f->type, 1);
			miku.write((char*)&f->distance, 4);
		}
		
		//***Extract Show IK Frame Info***
		unsigned showIKCount=vmdInfo.showIKFrames.size();
		miku.write((char*)&showIKCount, 4);
		
		for(int i=0; i < vmdInfo.showIKFrames.size(); ++i)
		{
			VMDShowIKFrame *f = &vmdInfo.showIKFrames[i];
			
			miku.write((char*)&f->frame, 4);
			miku.write((char*)&f->show, 1);
			
			
			unsigned IKCount=f->IKList.size();
			miku.write((char*)&IKCount, 4);
			
			for(int i=0; i<f->IKList.size(); ++i)
			{
				VMDIKInfo *info = &f->IKList[i];
				
				string name=UTF8ToSJIS(info->name.c_str());
				
				miku.write(name.c_str(), 20);
				miku.write((char*)&info->isOn, 1);
			}
		}
		
		miku.close();
	}
}
