#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include "pmx.h"
#include "vmd.h"

#include <list>
#include <sstream>

#include <OpenGL/gl.h>

extern glm::quat IKQuat;

struct VertexData;

/*!  \class VMDMotionController
 * \if ENGLISH
 * \brief Class for animating a PMX model using motion data from a VMD file.
 * 
 * Detailed description starts here(VMDMotionController).
 * \endif
 * 
 * \if JAPANESE
 * \brief VMDモーションデーターでPMXモデルを動かす為のクラス。
 * 
 * 詳細はここに（VMDMotionController）。
 * \endif
*/
class VMDMotionController
{
	public:
	/*! \if ENGLISH \brief Constructs a VMDMotionController instance.
	 * 
	 * @param pmxInfo The PMX Model to be animated.
	 * @param vmdInfo The VMD Motion to animate the model with.
	 * @param shaderProgram The shader program from which to pull the location for the uniform variable 'Bones' from.
	 * \endif
	 * \if JAPANESE \brief VMDMotionControllerのコンストラクタ。
	 * 
	 * @param pmxInfo 動かされるPMXモデル。
	 * @param vmdInfo モデルを動かすVMDモーション。
	 * @param shaderProgram Uniform変数「Bones」を取得出来るシェーダープログラム。
	 * \endif
	*/
	VMDMotionController(ClosedMMDFormat::PMXInfo &pmxInfo,ClosedMMDFormat::VMDInfo &vmdInfo,GLuint shaderProgram);			
	~VMDMotionController();
	void updateVertexMorphs(); 			//!< \if ENGLISH \brief A function used to update the Vertices of a model based on Vertex Morph animation parameters. \endif
										//!< \if JAPANESE \brief 頂点モーフのモーションパラメータを利用してモデルの頂点を更新する為の関数。 \endif
	void updateBoneMatrix();			//!< \if ENGLISH \brief A function used to update the uniform 'Bones' in the shader program shaderProgram with the model's bone transformation matrices. \endif
										//!< \if JAPANESE \brief Uniform変数「Bones」をモデルのボーン変形行列の配列で更新する為の関数。 \endif
	void updateBoneAnimation();			//!< \if ENGLISH \brief A function used to update the model's bone transformation matrices with the current frame of animation. \endif
										//!< \if JAPANESE \brief モデルのボーン変形行列の配列を更新する為の関数。 \endif
	/*!
	 * \if ENGLISH
	 * \brief A function used to move on to the next frame in the animation.
	 * \return true if animation is finished
	 * \endif
	 * \if JAPANESE
	 * \brief モーションの次のフレームへと進む為の関数。
	 * \return アニメーションが終了したらtrue
	 * \endif
	 */
	bool advanceTime();
	
	
	//public variables
	glm::mat4 *skinMatrix; 				//!< \if ENGLISH \brief A pointer to an array of bone transformation matrices that represent the model's current pose. \endif
										//!< \if JAPANESE \brief モデルの現在ポーズを表すボーン変形行列の配列へのポインター。 \endif
	glm::mat4 *invBindPose;				//!< \if ENGLISH \brief A pointer to an array of bone transformation matrices that represent the inverse of the model's initial pose. \endif
										//!< \if JAPANESE \brief モデルの初期ポーズの逆性を表すボーン変形行列の配列へのポインター。 \endif
	VertexData *vertexData;				//!< \if ENGLISH \brief A pointer to the array of vertices that the model is made of. \endif
										//!< \if JAPANESE \brief モデルが出来ている頂点の配列へのポインター。 \endif
	
	
	private:
	int time;
	
	void updateChildren(ClosedMMDFormat::PMXBone *linkBone);
	void updateIK();
	void solveIK(ClosedMMDFormat::PMXBone *joint, int effector, glm::vec3 targetPos, int numIteration, int numBones, int numMaxBones);
	
	
	ClosedMMDFormat::PMXInfo &pmxInfo;
	ClosedMMDFormat::VMDInfo &vmdInfo;
	
	private:
	GLuint Bones_loc; //uniform variable location for Bones[] in shader
	
	//***BONE TRANSFORMATION VARIABLES
	std::vector<std::list<ClosedMMDFormat::VMDBoneFrame> > boneKeyFrames;				//Hold a list of keyframes for each Bone
	std::vector<std::list<ClosedMMDFormat::VMDBoneFrame>::iterator> ite_boneKeyFrames;	//Keyframe iterator
	
	std::vector<glm::quat> boneRot;
	std::vector<glm::vec3> bonePos;	
	
	//***VERTEX MORPH VARIABLES
	std::vector<std::list<ClosedMMDFormat::VMDMorphFrame> > morphKeyFrames;				//Hold a list of keyframes for each vertex morph
	std::vector<std::list<ClosedMMDFormat::VMDMorphFrame>::iterator> ite_morphKeyFrames;	//Keyframe iterator
	
	std::vector<float> vMorphWeights;
};

/*! \struct VertexData
 * \if ENGLISH \brief Struct for holding MMD Model Vertex Information. \endif
 * \if JAPANESE \brief MMDモデルの頂点情報を持つ為の構造体。 \endif　*/
struct VertexData
{
	glm::vec4 position;
	glm::vec2 UV;
	glm::vec3 normal;

	GLfloat weightFormula;

	GLfloat boneIndex1;
	GLfloat boneIndex2;
	GLfloat boneIndex3;
	GLfloat boneIndex4;

	GLfloat weight1;
	GLfloat weight2;
	GLfloat weight3;
	GLfloat weight4;

	std::string str()
	{	
		std::stringstream ss;
		ss << position.x << " " << position.y << " " << position.z << " " << position.w << std::endl;
		ss << UV.x << " " << UV.y << std::endl;
		ss << normal.x << " " << normal.y << " " << normal.z << std::endl;
		ss << boneIndex1 << " " << boneIndex2 << " " << boneIndex3 << " " << boneIndex4 << std::endl;
		ss << weight1 << " " << weight2 << " " << weight3 << " " << weight4 << std::endl;

		return ss.str();
	}
};



#endif
