#ifndef INTERPOLATION_H
#define INTERPOLATION_H

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/euler_angles.hpp>


/*! \struct BezierParameters
 * \if ENGLISH \brief Struct for holding Bezier Curve Interpolation Information.
 * 
 * It is assumed that p0=(0,0) and p3(127,127). \n
 * For each axis(X,Y,Z) and rotation(R) 2 other points, p1 and p2, are defined to specify the curve. \endif
 * \if JAPANESE \brief ベジェ曲線情報を持つ為の構造体。
 * 
 * p0=(0,0)とp3(127,127)は前提として、 \n
 * 各軸（X、Y、Z）と回転（R）はもう一つの点、p1とp2で曲線が特定されます。
 \endif　*/
struct BezierParameters
{
	//See "On VMD Interpolation Parameters.txt" in DOCs for information concerning bezier parameters.
	
	glm::vec2 X1;	//!< \if ENGLISH \brief p1 for the X-axis. \endif
			//!< \if JAPANESE \brief X軸のp1。 \endif
	glm::vec2 X2;	//!< \if ENGLISH \brief p2 for the X-axis. \endif
			//!< \if JAPANESE \brief X軸のp2。 \endif
	
	
	glm::vec2 Y1;	//!< \if ENGLISH \brief p1 for the Y-axis. \endif
			//!< \if JAPANESE \brief Y軸のp1。 \endif
	glm::vec2 Y2;	//!< \if ENGLISH \brief p2 for the Y-axis. \endif
			//!< \if JAPANESE \brief Y軸のp2。 \endif
	
	
	glm::vec2 Z1;	//!< \if ENGLISH \brief p1 for the Z-axis. \endif
			//!< \if JAPANESE \brief Z軸のp1。 \endif
	glm::vec2 Z2;	//!< \if ENGLISH \brief p2 for the Z-axis. \endif
			//!< \if JAPANESE \brief Z軸のp2。 \endif
	
	
	glm::vec2 R1;	//!< \if ENGLISH \brief p1 for the rotation. \endif
			//!< \if JAPANESE \brief 回転のp1。 \endif
	glm::vec2 R2;	//!< \if ENGLISH \brief p2 for the rotation. \endif
			//!< \if JAPANESE \brief 回転のp2。 \endif
};

float bezier(float X, float x1,float y1, float x2,float y2);

glm::fquat Slerp(glm::quat &v0, glm::quat &v1, float alpha);
glm::vec4 Vectorize(const glm::quat theQuat);

glm::vec4 Lerp(const glm::vec4 &start, const glm::vec4 &end, float alpha);
glm::vec4 Slerp(glm::vec4 &v0, glm::vec4 &v1, float alpha);

float Lerp(const float &start, const float &end, float alpha);
float Slerp(float &v0, float &v1, float alpha);

#endif
