#ifndef MMD_PHYSICS_H
#define MMD_PHYSICS_H

#include "bulletphysics.h"
#include "pmx.h"
#include "vmd.h"
#include "motioncontroller.h"

#include <vector>
#include <string>
#include <fstream>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
/*!  \class MMDPhysics
 * \if ENGLISH
 * \brief Class for physics calculations on MMD rigid bodies.
 * 
 * Detailed description starts here(MMDPhysics).
 * \endif
 * 
 * \if JAPANESE
 * \brief MMD剛体に物理演算を行う為のクラス。
 * 
 * 詳細はここに（MMDPhysics）。
 * \endif
 */
class MMDPhysics
{
	public:
	/*! \if ENGLISH \brief Constructs a MMDPhysics instance.
	 * 
	 * @param pmxInfo The PMX Model to be animated.
	 * @param motionController The VMDMotionController instance being used to animate the model
	 * @param bulletPhysics The BulletPhysics instance to be used for physics calculation.
	 * \endif
	 * \if JAPANESE \brief MMDPhysicsのコンストラクタ。
	 * 
	 * @param pmxInfo 動かされるPMXモデル。
	 * @param motionController モデルを動かす為に使われているVMDMotionController。
	 * @param bulletPhysics 物理演算に使われるBulletPhysics。
	 * \endif
	*/
	MMDPhysics(ClosedMMDFormat::PMXInfo &pmxInfo, VMDMotionController *motionController, BulletPhysics* bulletPhysics);

	/*! \if ENGLISH \brief Runs 1 frame of rigidbody physics simulation and updates the related bones in the model.
	 * 
	 * @param physicsEnabled Set whether to do physics calculations.
	 * \endif
	 * \if JAPANESE \brief 剛体による物理演算を一フレーム実行して、剛体の関連ボーンを更新する。
	 * 
	 * @param physicsEnabled 物理演算を行うかどうかを決める。
	 * \endif
	*/
	void updateBones(bool physicsEnabled);
	
	
	private:
	glm::mat4 createRigidMatrix(const glm::vec3 &pos, const glm::vec3 &rot);
	
	void createRigidBody();
	void createJoints();
	
	
	
	std::vector<int> rigidMeshIndices;
	std::vector<std::vector<glm::vec3> > rigidMeshes;
	
	std::vector<std::vector<glm::vec3> > jointMeshes;
	
	std::vector<btRigidBody*> rigidBodies;
	
	BulletPhysics* bulletPhysics;
	VMDMotionController *motionController;
	ClosedMMDFormat::PMXInfo &pmxInfo;
};


#endif
