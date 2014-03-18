#ifndef BULLET_DEBUG_DRAWER_H
#define BULLET_DEBUG_DRAWER_H

#include "btBulletDynamicsCommon.h"

#include <vector>
#include <string>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <OpenGL/gl.h>



struct btDebugVertexData;


/*! \class BulletDebugDrawer
 * \if ENGLISH \brief Class for drawing debug info provided by Bullet.
 * 
 * You can learn more about Bullet's debug drawing <a href=http://bulletphysics.org/Bullet/BulletFull/classbtIDebugDraw.html>here</a>
 * \endif
 * 
 * \if JAPANESE \brief Bulletが提供するデバッグ情報を表示する為のクラス。
 * 
 * Bulletのデバッグ表示の詳細について： <a href=http://bulletphysics.org/Bullet/BulletFull/classbtIDebugDraw.html>ここ</a>
 * \endif
*/
class BulletDebugDrawer: public btIDebugDraw
{
	public:
	/*! \if ENGLISH \brief Class Constructor.
	 * 
	 * Shaders to be used with this class are provided in shaders as bulletDebug.vert and bulletDebug.frag.
	 * @param shaderProgram The OpenGL ID for the Bullet Shader Program that will draw the debug info \endif
	 * 
	 * \if JAPANESE \brief クラスのコンストラクタ。
	 * 
	 * このクラスと一緒に使うシェ－ダーはshadersフォルダにあります（bulletDebug.vert、bulletDebug.frag）
	 * 
	 * @param shaderProgram Bulletのデバッグ情報を描くシェ－ダープログラムのID（OpenGLから取得） \endif
	*/
	BulletDebugDrawer(std::string vertexShaderPath, std::string fragmentShaderPath);
	
	/*! \if ENGLISH \brief Draws a line from Bullet.
	 * 
	 * @param from The first point drawn
	 * @param to The second point drawn
	 * @param The color of the line
	 * 
	 * \endif
	 * \if JAPANESE \brief Bulletからの線を書ける。
	 * 
	 * @param from 最初の点
	 * @param to ２つ目の点
	 * @param color 線の色
	 * \endif
	*/
	void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
	
	/*! \if ENGLISH @name Unimplemented Debug Functions 
	 * \todo Complete BulletDebugDrawer class.
	 * 
	 * \endif
	 * \if JAPANESE @name まだ実装されていないデバッグ表示関数
	 * \todo BulletDebugDrawerクラスを完成させる。
	 * 
	 * \endif
	*/
	//@{
	void drawContactPoint (const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color);
	void draw3dText(const btVector3 &location, const char *textString);
	//@}
	
	void reportErrorWarning (const char *warningString); 	//!< \if ENGLISH \brief Print a warning message from Bullet. \endif
								//!< \if JAPANESE \brief Bulletからのワーニングを表示する。 \endif
	
	/*! \if ENGLISH \brief Sets the mode for Bullet's debug display.
	 * 
	 * For details on the different debug modes: <a href=http://bulletphysics.org/Bullet/BulletFull/classbtIDebugDraw.html#ab4bb43fbfefb526c552c0943fac5832a>here</a>
	 * @param debugMode The debug mode to be used
	 * \endif
	 * \if JAPANESE \brief Bulletのデバッグ表示のモードを特定する。
	 * 
	 * デバッグモードの種類について：　<a href=http://bulletphysics.org/Bullet/BulletFull/classbtIDebugDraw.html#ab4bb43fbfefb526c552c0943fac5832a>ここ</a>
	 * @param debugMode これから使うデバッグモード。
	 * \endif
	*/
	void setDebugMode (int debugMode);
	
	int getDebugMode()const;	//!< \if ENGLISH \brief Get the current debug mode. \endif
					//!< \if JAPANESE \brief 今利用しているデバッグモードを取得。 \endif
	
	void render(); 				//!< \if ENGLISH \brief Push geometry to the VAO and render. \endif
						//!< \if JAPANESE \brief ジオメトリをVAOに読み込んで、レンダリングを行う。 \endif
	
	
	GLuint shaderProgram;
	GLuint MVPLoc;
	private:
	int m_debugMode;
	
	unsigned int VAO;
	unsigned int VertexArrayBuffer;
	
	std::vector<glm::vec4> lines;
	std::vector<glm::vec4> lineColors;
	
	std::vector<btDebugVertexData> btDebugVertices;
	
	static const float vertices[3][2];
};

/*! \struct btDebugVertexData
 * \if ENGLISH \brief Struct for holding Bullet Debug Vertex Information. \endif
 * \if JAPANESE \brief Bulletデバッグの頂点情報を持つ為の構造体。 \endif　*/
struct btDebugVertexData
{
	glm::vec4 position;	//!< \if ENGLISH \brief The vertex's position. \endif
				//!< \if JAPANESE \brief 頂点の位置。 \endif
	glm::vec4 color;	//!< \if ENGLISH \brief The vertex's color(RGBA) \endif
				//!< \if JAPANESE \brief 頂点の色（RGBA） \endif
};


#endif
