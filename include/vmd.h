#ifndef VMD_H
#define VMD_H

#include <vector>
#include <string>
#include <fstream>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include "interpolation.h"
//VMD related structs

namespace ClosedMMDFormat
{
	struct VMDInfo;
	VMDInfo &readVMD(std::string filename);
	void writeVMD(VMDInfo &vmdInfo, std::string filename);

	struct VMDBoneFrame
	{
		std::string name; //char[15] before UTF8 conversion
		
		unsigned frame;
		
		glm::vec3 translation; //values will be 0 when there's no position
		glm::quat rotation; //values will be 0 when there's no position (w will be 1.0)
		
		BezierParameters bezier;
		
		bool operator < (const VMDBoneFrame &k) const
		{
			//Comparison by frame number
			return frame < k.frame;
		}
	};

	struct VMDMorphFrame
	{
		std::string name; //char[15] before UTF8 conversion
		unsigned frame;
		float value;
		
		bool operator < (const VMDMorphFrame &k) const
		{
			//Comparison by frame number
			return frame < k.frame;
		}
	};

	struct VMDCameraFrame
	{
		unsigned frame;
		
		float distance; //Distance between the camera and pt of interest (the camera is negative when in front of the object)
		glm::vec3 position;
		glm::vec3 rotation;
		
		//TODO: Read Camera Frame Bezier Parameters correctly
		char interpolationParameters[24];
		
		unsigned int viewAngle; //viewing angle in degrees
		bool perspective; //0:ON 1:OFF
	};

	struct VMDLightFrame
	{
		unsigned frame;
		glm::vec3 color;
		glm::vec3 position;
	};

	struct VMDSelfShadowFrame
	{
		unsigned frame;
		uint8_t type; // self shadow type(0:OFF 1:Mode1 2:Mode2)
		float distance;
	};
	
	struct VMDIKInfo
	{
		std::string name; //char[20] before UTF8 conversion
		bool isOn; //0:OFF 1:ON
	};
	
	struct VMDShowIKFrame
	{
		unsigned frame;
		bool show; //0:OFF 1:ON
		
		std::vector<VMDIKInfo> IKList;
	};

	/*!  \class VMDInfo
	 * \if ENGLISH
	 * \brief Class for loading/storing info about a VMD motion file.
	 * 
	 * \endif
	 * 
	 * \if JAPANESE
	 * \brief VMDモーションを読み込む／格納する為のクラス。
	 * 
	* \endif
	*/
	struct VMDInfo
	{
		std::string modelName; //char[20] before UTF8 conversion
		
		std::vector<VMDBoneFrame> boneFrames;
		std::vector<VMDMorphFrame> morphFrames;
		std::vector<VMDCameraFrame> cameraFrames;
		std::vector<VMDLightFrame> lightFrames;
		std::vector<VMDSelfShadowFrame> selfShadowFrames;
		std::vector<VMDShowIKFrame> showIKFrames;
	};
}

#endif
