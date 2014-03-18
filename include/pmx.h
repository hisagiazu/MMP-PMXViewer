#ifndef PMX_H
#define PMX_H

#include <vector>
#include <string>
#include <fstream>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#define VERTEX_DEBUG false
#define MATERIAL_DEBUG false

/*! \namespace ClosedMMDFormat
 * \if ENGLISH \brief Namespace for closed/proprietary MMD File Format handlers.
 * \endif
 * 
 * \if JAPANESE \brief クローズド／独自開発されたMMDファイル形式用のnamespace。
 * 
 * Bulletのデバッグ表示の詳細について： <a href=http://bulletphysics.org/Bullet/BulletFull/classbtIDebugDraw.html>ここ</a>
 * \endif
*/

namespace ClosedMMDFormat
{
	enum WeightFormula
	{
		WEIGHT_FORMULA_BDEF1=0,
		WEIGHT_FORMULA_BDEF2=1,
		WEIGHT_FORMULA_BDEF4=2,
		WEIGHT_FORMULA_SDEF=3
	};

	enum MorphType
	{
		MORPH_TYPE_GROUP=0,
		MORPH_TYPE_VERTEX=1,
		MORPH_TYPE_BONE=2,
		MORPH_TYPE_UV=3,
		MORPH_TYPE_EXTRA_UV1=4,
		MORPH_TYPE_EXTRA_UV2=5,
		MORPH_TYPE_EXTRA_UV3=6,
		MORPH_TYPE_EXTRA_UV4=7,
		MORPH_TYPE_MATERIAL=8
	};

	enum RigidBodyShapeType
	{
		RIGID_SHAPE_SPHERE=0,
		RIGID_SHAPE_CUBE=1,
		RIGID_SHAPE_CAPSULE=2
	};

	//!< \if ENGLISH rigid body type \endif
	//!< \if JAPANESE 剛体タイプ \endif
	enum RigidBodyType
	{
		RIGID_TYPE_FOLLOWER=0,		//!< \if ENGLISH bone follower type \endif
									//!< \if JAPANESE ボーン追従タイプ \endif
		RIGID_TYPE_PHYSICS=1,		//!< \if ENGLISH physics type \endif
									//!< \if JAPANESE 物理演算タイプ \endif
		RIGID_TYPE_PHYSICS_ROT_ONLY=2	//!< \if ENGLISH physics (rotation only) type \endif
										//!< \if JAPANESE 物理+ボーン位置合わせタイプ \endif
	};

	enum PmxEncode
	{
		PMX_ENCODE_UTF16=0,
		PMX_ENCODE_UTF8=1
	};

	struct PMXBone;
	struct PMXInfo;
	
	//PMX related Functions
	void getPMXText(std::ifstream &miku, PMXInfo &pmxInfo, std::string &result, bool debug=false);
	PMXInfo &readPMX(std::string foldername,std::string filename);
	
	/*!  \class PMXVertex
	 * \if ENGLISH
	 * \brief A struct for holding information about a vertex in a PMX file.
	 * 
	 * Detailed description starts here(PMXVertex).
	 * \endif
	 * 
	 * \if JAPANESE
	 * \brief PMXファイルに入っている頂点情報を格納する為の構造体。
	 * 
	 * 詳細はここに（PMXVertex）。
	 * \endif
	*/
	struct PMXVertex
	{
		glm::vec3 pos;						//!< \if ENGLISH \brief Vertex Position \endif
											//!< \if JAPANESE \brief 頂点の位置 \endif
		glm::vec3 normal;					//!< \if ENGLISH \brief Vertex Normal \endif
											//!< \if JAPANESE \brief 頂点のノーマル \endif
		glm::vec2 UV; 						//!< \if ENGLISH \brief Texture UV Coordinates \endif
											//!< \if JAPANESE \brief テクスチャのUV座標 \endif
		
		uint8_t weight_transform_formula;	//!< \if ENGLISH \brief Weight Transformation Formula \endif
											//!< \if JAPANESE \brief ボーンのウエイト変形形式 \endif
											/*!< 0:BDEF1 \n
											 * 1:BDEF2 \n
											 * 2:BDEF4 \n
											 * 3:SDEF */
		
		//Bone Info
		int boneIndex1;						//!< \if ENGLISH \brief The Index of Bone 1 (For bone transformation) \endif
											//!< \if JAPANESE \brief ボーン１のインデクス（ボーン変形用） \endif
		int boneIndex2;						
		int boneIndex3;						
		int boneIndex4;						
		
		float weight1;						//!< \if ENGLISH \brief The weight value of Bone 1 (For bone transformation) \endif
											//!< \if JAPANESE \brief ボーン１の重み値（ボーン変形用） \endif
		float weight2;
		float weight3;
		float weight4;
		
		//SDEF Bone Info
		glm::vec3 C;
		glm::vec3 R0;
		glm::vec3 R1;
		
		float edgeScale;
	};

	struct PMXFace
	{
		unsigned points[3]; //Point indices
	};

	struct PMXMaterial
	{
		std::string name;
		std::string nameEng;
		
		glm::vec4 diffuse;
		glm::vec3 specular;
		float shininess; //Specular Coefficient
		glm::vec3 ambient;
		
		bool drawBothSides;
		bool drawGroundShadow;
		bool drawToSelfShadowMap;
		bool drawSelfShadow;
		bool drawEdges;
		
		glm::vec4 edgeColor;
		float edgeSize;
		
		int textureIndex;
		int sphereIndex;
		
		uint8_t sphereMode; //0:no effect 1:multiplication (sph), 2:addition(spa), 3:subtexture (use extra UV1's X,Y as a UV reference for normal texture drawing)
		uint8_t shareToon; //0:Next variable is separate toon 1:Next variable is shared Toon
		
		int toonTextureIndex; //if shareToon=0, use texture at this texture index
		uint8_t shareToonTexture; //if shareToon=1, use Shared Toon Textures. Values [0~9] correspond with toon01.bmp->toon10.bmp
		
		std::string memo; //various comments
		
		int hasFaceNum; //Number of Indices that have the material (always a multiple of 3)
	};

	struct PMXIKLink
	{
		int linkBoneIndex;
		
		bool angleLimit; //0:OFF 1:ON
		//If angleLimit==1
		glm::vec3 lowerLimit; //lowerLimit (x,y,z) -> Radian Angle
		glm::vec3 upperLimit; //upperLimit (x,y,z) -> Radian Angle
	};

	struct PMXBone
	{
		/*Notes concerning PMXBones:
		 * When a given bone is translated/rotated, all of its children are translated/rotated
		 * 
		 * When Rotation/Movement allowed is turned on, the bone's givenParentBone's movement/rotation values get added like so:
		 * givenParentBone(x,y,z) -> thisBone(+allowRate*x, +allowRate*y, +allowRate*z)
		 * (Depending on the allowRate value, negative values also become possible)
		 * 
		 * */
		
		std::string name;
		std::string nameEng;
		
		glm::vec3 position;
		
		int parentBoneIndex; //Set to -1 when unused
		
		int transformationLevel; //used as part of determining order in which transformations are applied
		
		bool connectionDisplayMethod; //Joint display method (PMD Child Bone Selection) 0: With a coordinate offset 1: With a bone
		bool rotationPossible;
		bool movementPossible;
		bool show;
		bool controlPossible;
		bool IK; //USED IN TRANSFORMATIONS
		
		bool giveRotation; //USED IN TRANSFORMATIONS
		bool giveTranslation; //USED IN TRANSFORMATIONS
		bool axisFixed;
		bool localAxis;
		bool transformAfterPhysics; //USED IN TRANSFORMATIONS; used as part of determining order in which transformations are applied
		bool externalParentTransform; //USED IN TRANSFORMATIONS
		
		//If connectionDisplayMethod==0
		glm::vec3 coordinateOffset; //Relative to the Bone position
		
		//If connectionDisplayMethod==1
		int connectionBoneIndex;
		
		//If giveRotation==1 or giveTranslation==1
		int givenParentBoneIndex;
		float giveRate;
		
		//If axisFixed==1
		glm::vec3 axisDirectionVector;
		
		//If localAxis==1
		glm::vec3 XAxisDirectionVector;
		glm::vec3 ZAxisDirectionVector;
		
		//If externalParentTransform==1
		int keyValue;
		
		//If IK==1
		int IKTargetBoneIndex;
		unsigned IKLoopCount; //In PMD and MMD environments, this caps at 255 apparently
		float IKLoopAngleLimit; //At the time of loop calculation, angle limit each time the loop is run (Warning: 4x the value it was in PMD files)
		
		unsigned IKLinkNum; //Number of continuing IK(Inverse Kinetics) elements
		std::vector<PMXIKLink*> IKLinks;
		
		
		//VARIABLES I ADDED BELOW THIS POINT
		glm::mat4 Local; //Bone Transformation Matrix relative to the bone's parent bone.
		
		PMXBone *parent;
		
		glm::mat4 calculateGlobalMatrix()
		{
			if(parent) return parent->calculateGlobalMatrix() * Local;
			else return Local;
		}
	};

	struct PMXMorphData
	{
		//Only inherited classes used
	};

	struct PMXVertexMorph: PMXMorphData
	{
		int vertexIndex;
		glm::vec3 coordinateOffset;
	};

	struct PMXUVMorph: PMXMorphData
	{
		int vertexIndex;
		glm::vec4 UVOffsetAmount; //For normal UVs, z and w become unneeded data, but still get stored as part of morph data
	};

	struct PMXBoneMorph: PMXMorphData
	{
		int boneIndex;
		glm::vec3 inertia; //interia
		glm::vec4 rotationAmount; //Quarternion (x,y,z,w)
	};

	struct PMXMaterialMorph: PMXMorphData
	{
		int materialIndex; //-1: For all materials
		uint8_t offsetCalculationFormula; //0:Multiplication 1:Addition
		
		glm::vec4 diffuse; //Default values are: Multiplication: 1.0, Addition: 0.0 (same for below variables)
		
		glm::vec3 specular;
		float shininess; //Specular coefficient
		
		glm::vec3 ambient;
		
		glm::vec4 edgeColor;
		float edgeSize;
		
		glm::vec4 textureCoefficient; //(R,G,B,A)
		glm::vec4 sphereTextureCoefficient;
		glm::vec4 toonTextureCoefficient;
	};

	struct PMXGroupMorph: PMXMorphData
	{
		int morphIndex; //Note: Group morphs can not be made into group morphs
		float morphRate; //Group morph's morph points * morphRate = target morph's morph points
	};

	struct PMXMorph
	{
		std::string name;
		std::string nameEng;
		
		uint8_t controlPanel; //(PMD:Category) 1:Eyebrow(bottom left) 2:Eye(upper left) 4:Other(bottom right) 0:System reserved
		uint8_t type; //Morph type 0:Group 1:Vertex 2:Bone 3:UV 4:Extra UV1, 5:Extra UV 2, 6:Extra UV 3, 7:Extra UV4, 8:Material
		
		int morphOffsetNum; //Number of continuing offset data
		
		std::vector<PMXMorphData*> offsetData;
	};

	struct PMXDisplayFrameElement
	{
		bool target; //0: Bone 1:Morph
		unsigned int index;
	};

	struct PMXDisplayFrame
	{
		std::string name;
		std::string nameEng;
		
		bool specialFrameFlag; //0:Normal Frame 1:Special Frame
		int elementsWithinFrame; //Number of continuing elements
		
		std::vector<PMXDisplayFrameElement*> elements;
	};

	struct PMXRigidBody
	{
		std::string name;
		std::string nameEng;
		
		int relatedBoneIndex; //Set to -1 when irrelevant/unrelated
		
		uint8_t group;
		unsigned short noCollisionGroupFlag;
		
		uint8_t shape; //0:Circle 1:Square 2:Capsule
		glm::vec3 size; //(x,y,z)
		
		glm::vec3 position;
		glm::vec3 rotation; //->radian angle
		
		float mass;
		float movementDecay; //movement reduction
		float rotationDecay; //rotation reduction
		float elasticity; //recoil
		float friction; //strength of friction
		
		uint8_t physicsOperation; //0:Follow Bone (static), 1:Physics Calc. (dynamic), 2: Physics Calc. + Bone position matching
		
		
		//VARIABLES I ADDED BELOW THIS POINT
		glm::mat4 Init; //The initial transformation matrix
		glm::mat4 Offset; //The offset matrix (the inverse of the initial transformation matrix)
	};

	struct PMXJoint
	{
		std::string name;
		std::string nameEng;
		
		bool type; //0:Spring 6DOF; in PMX 2.0 always set to 0 (included to give room for expansion)
		
		//If joint type==0
		int relatedRigidBodyIndexA; //-1 if irrelevant
		int relatedRigidBodyIndexB;
		
		//Spring position/rotation
		glm::vec3 position;
		glm::vec3 rotation;
		
		glm::vec3 movementLowerLimit;
		glm::vec3 movementUpperLimit;
		glm::vec3 rotationLowerLimit; //-> radian angle
		glm::vec3 rotationUpperLimit; //-> radian angle
		
		glm::vec3 springMovementConstant;
		glm::vec3 springRotationConstant;
		
		//VARIABLES I ADDED BELOW THIS POINT
		glm::mat4 Local; //joint matrix, local to relatedRigidBodyA
	};
	
	
	/*!  \class PMXInfo
	 * \if ENGLISH
	 * \brief Class for loading/storing info about a PMX model.
	 * 
	 * \endif
	 * 
	 * \if JAPANESE
	 * \brief PMXモデルを読み込む／格納する為のクラス。
	 * 
	* \endif
	*/
	struct PMXInfo
	{
		//For more info, see the PMX documentation included with PMDEditor
		//also http://gulshan-i-raz.geo.jp/labs/2012/10/17/pmx-format1/
		
		//Header
		char header_str[4]; 							//!< \if ENGLISH \brief PMX Header string. Should read 'PMX ' for versions 2.0+, 'Pmx ' for version 1.0. \endif
														//!< \if JAPANESE \brief PMXのヘッダー文字列。バージョン2.0+なら「PMX 」、バージョン1.0なら「Pmx 」であるべき。 \endif
		float ver; 										//!< \if ENGLISH \brief PMX Version Number. \endif
														//!< \if JAPANESE \brief PMXバージョン番号。 \endif
		
		uint8_t line_size; 								//!< \if ENGLISH \brief The size (in bytes) of the next line in the PMX file that contains various flags. \endif
														//!< \if JAPANESE \brief 様々なフラグが格納されている次の列のサイズ（バイト数）。 \endif
		bool unicode_type;								//!< \if ENGLISH \brief The unicode encoding for text in the PMX file (0:UTF16, 1:UTF8) \endif
														//!< \if JAPANESE \brief PMXファイルのエンコーディング式（0：UTF16、1:UTF8） \endif
		
		uint8_t extraUVCount;							//!< \if ENGLISH \brief  Extra UV coordinates (range of 0~4) \endif
														//!< \if JAPANESE \brief 追加UVの数（0~4） \endif
		
		uint8_t vertexIndexSize;						//!< \if ENGLISH \brief  The size of an index that references a tertex (1,2,4 bytes). \endif
														//!< \if JAPANESE \brief 頂点を参照するインデクスのサイズ（1,2,4バイト）。 \endif
		uint8_t textureIndexSize;						//!< \if ENGLISH \brief  The size of an index that references a texture (1,2,4 bytes). \endif
														//!< \if JAPANESE \brief 頂点を参照するインデクスのサイズ（1,2,4バイト）。 \endif
		uint8_t materialIndexSize;						//!< \if ENGLISH \brief  The size of an index that references a material (1,2,4 bytes). \endif
														//!< \if JAPANESE \brief マテリアルを参照するインデクスのサイズ（1,2,4バイト）。 \endif
		uint8_t boneIndexSize;							//!< \if ENGLISH \brief  The size of an index that references a bone (1,2,4 bytes). \endif
														//!< \if JAPANESE \brief ボーンを参照するインデクスのサイズ（1,2,4バイト）。 \endif
		uint8_t morphIndexSize;							//!< \if ENGLISH \brief  The size of an index that references a morph (1,2,4 bytes). \endif
														//!< \if JAPANESE \brief モーフを参照するインデクスのサイズ（1,2,4バイト）。 \endif
		uint8_t rigidBodyIndexSize;						//!< \if ENGLISH \brief  The size of an index that references a rigidbody (1,2,4 bytes). \endif
														//!< \if JAPANESE \brief 剛体を参照するインデクスのサイズ（1,2,4バイト）。 \endif
		
		//Model Info
		std::string modelName;							//!< \if ENGLISH \brief  The name of the model (in Japanese) \endif
														//!< \if JAPANESE \brief モデルの名前（日本語で）。 \endif
		std::string modelNameEnglish;					//!< \if ENGLISH \brief  The name of the model (in English) \endif
														//!< \if JAPANESE \brief モデルの名前（英語で）。 \endif
		std::string comment;							//!< \if ENGLISH \brief  Comments about the model (in Japanese) \endif
														//!< \if JAPANESE \brief モデルに関するコメント（日本語で）。 \endif
		std::string commentEnglish;						//!< \if ENGLISH \brief  Comments about the model (in English) \endif
														//!< \if JAPANESE \brief モデルに関するコメント（英語で）。 \endif
		
		//Vertex
		int vertex_continuing_datasets; 				//!< \if ENGLISH \brief  The number of vertices in the PMX model. \endif
														//!< \if JAPANESE \brief PMXモデルに格納されている頂点の数。 \endif
		std::vector<PMXVertex*> vertices;				//!< \if ENGLISH \brief  A vector containing the vertices. \endif
														//!< \if JAPANESE \brief 頂点の配列（vector式） \endif
		
		//Face
		int face_continuing_datasets;					//!< \if ENGLISH \brief  The number of faces in the PMX model. \endif
														//!< \if JAPANESE \brief PMXモデルに格納されている面の数。 \endif
		std::vector<PMXFace*> faces;					//!< \if ENGLISH \brief  A vector containing the faces. \endif
														//!< \if JAPANESE \brief 面の配列（vector式） \endif
		
		//Texture
		int texture_continuing_datasets;				//!< \if ENGLISH \brief  The number of textures in the PMX model. \endif
														//!< \if JAPANESE \brief PMXモデルに格納されているテクスチャの数。 \endif
		std::string *texturePaths;						//!< \if ENGLISH \brief  A pointer containing an array texture paths. \endif
														//!< \if JAPANESE \brief テクスチャのパスの配列へのポインター。 \endif
		
		//Material
		int material_continuing_datasets;				//!< \if ENGLISH \brief  The number of materials in the PMX model. \endif
														//!< \if JAPANESE \brief PMXモデルに格納されているマテリアルの数。 \endif
		std::vector<PMXMaterial*> materials;			//!< \if ENGLISH \brief  A vector containing the materials. \endif
														//!< \if JAPANESE \brief マテリアルの配列（vector式） \endif
		
		//Bone
		int bone_continuing_datasets;					//!< \if ENGLISH \brief  The number of bones in the PMX model. \endif
														//!< \if JAPANESE \brief PMXモデルに格納されているボーンの数。 \endif
		std::vector<PMXBone*> bones;					//!< \if ENGLISH \brief  A vector containing the bones. \endif
														//!< \if JAPANESE \brief ボーンの配列（vector式） \endif
		
		//Morph (Emotion data)
		int morph_continuing_datasets;					//!< \if ENGLISH \brief  The number of morphs in the PMX model. \endif
														//!< \if JAPANESE \brief PMXモデルに格納されているモーフの数。 \endif
		std::vector<PMXMorph*> morphs;					//!< \if ENGLISH \brief  A vector containing the bones. \endif
														//!< \if JAPANESE \brief モーフの配列（vector式） \endif
		
		//Display Frame
		int display_frame_continuing_datasets;			//!< \if ENGLISH \brief  The number of display frames in the PMX model. \endif
														//!< \if JAPANESE \brief PMXモデルに格納されている表示枠の数。 \endif
		std::vector<PMXDisplayFrame*> displayFrames;	//!< \if ENGLISH \brief  A vector containing the display frames. \endif
														//!< \if JAPANESE \brief 表示枠の配列（vector式） \endif
		
		//Rigid Body
		int rigid_body_continuing_datasets;				//!< \if ENGLISH \brief  The number of rigidbodies in the PMX model. \endif
														//!< \if JAPANESE \brief PMXモデルに格納されている剛体の数。 \endif
		std::vector<PMXRigidBody*> rigidBodies;			//!< \if ENGLISH \brief  A vector containing the rigidbodies. \endif
														//!< \if JAPANESE \brief 剛体の配列（vector式） \endif
		
		//Joint
		int joint_continuing_datasets;					//!< \if ENGLISH \brief  The number of joints in the PMX model. \endif
														//!< \if JAPANESE \brief PMXモデルに格納されているジョイントの数。 \endif
		std::vector<PMXJoint*> joints;					//!< \if ENGLISH \brief  A vector containing the joints. \endif
														//!< \if JAPANESE \brief ジョイントの配列（vector式） \endif
	};

	std::ostream& operator<<(std::ostream&, const PMXIKLink&);
	std::ostream& operator<<(std::ostream&, const PMXBone&);
	std::ostream& operator<<(std::ostream&, const PMXRigidBody&);
	std::ostream& operator<<(std::ostream&, const PMXJoint&);
}

#endif
