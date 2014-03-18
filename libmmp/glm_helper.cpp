#include "glm_helper.h"
#include <iostream>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace
{
	float safe_asin(float x)
	{
		return asin(glm::clamp<float>(x, -1, +1));
	}
}

std::ostream&
operator<<(std::ostream& rOut, const glm::vec3& v)
{
	rOut << v[0] << ", " << v[1] << ", " << v[2];
	return rOut;
}

std::ostream&
operator<<(std::ostream& rOut, const glm::vec4& v)
{
	rOut << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3];
	return rOut;
}

std::ostream&
operator<<(std::ostream& rOut, const glm::quat& v)
{
	rOut << v.w << ", " << v.x << ", " << v.y << ", " << v.z;
	return rOut;
}

std::ostream&
operator<<(std::ostream& rOut, const glm::mat4& v)
{
	for (int i = 0; i < 4; ++i)
	{
		rOut << v[i][0] << ", " << v[i][1] << ", " << v[i][2] << ", " << v[i][3];
		if (i != 3)
		{
			rOut << "\n";
		}
	}
	return rOut;
}

void
flipZAxisOfRotationalLimits(glm::vec3& rLower, glm::vec3& rUpper)
{
	// These are used to clamp joint limits into proper range of euler angles.
	static const glm::vec3 euler_lower(-M_PI + FLT_EPSILON, -0.5 * M_PI + FLT_EPSILON, -M_PI + FLT_EPSILON);
	static const glm::vec3 euler_upper(-euler_lower);

	// These clampings are needed.
	// Because lower/upper limits of existing PMX files tend to go slightly outside of ranges of euler angles.
	rLower = glm::clamp(rLower, euler_lower, euler_upper);
	rUpper = glm::clamp(rUpper, euler_lower, euler_upper);

	rLower = flipZAxisOfEulerAnglesRadians(rLower);
	rUpper = flipZAxisOfEulerAnglesRadians(rUpper);

	if (rUpper.x < rLower.x) std::swap(rUpper.x, rLower.x);
	if (rUpper.y < rLower.y) std::swap(rUpper.y, rLower.y);
	if (rUpper.z < rLower.z) std::swap(rUpper.z, rLower.z);
}

glm::vec3
flipZAxisOfEulerAnglesRadians(const glm::vec3& eulerAngles)
{
	const glm::quat q = fromEulerAnglesRadians(eulerAngles);
	return toEulerAnglesRadians(flipZAxisOfQuaternion(q));
}

glm::quat
flipZAxisOfQuaternion(const glm::quat& q)
{
	return glm::quat(q.w, -q.x, -q.y, q.z);
}

glm::vec3
toEulerAnglesRadians(const glm::quat& q)
{
	return glm::vec3(atan2(2*(q.y*q.z+q.w*q.x), 1-2*(q.x*q.x+q.y*q.y)),
					-safe_asin(2*(q.x*q.z-q.w*q.y)),
					atan2(2*(q.x*q.y+q.w*q.z), 1-2*(q.y*q.y+q.z*q.z)));
}

glm::quat
fromEulerAnglesRadians(const glm::vec3& eulerAngles)
{
	const glm::quat qx = fromAxisAngleRadians(glm::vec3(1, 0, 0), eulerAngles.x);
	const glm::quat qy = fromAxisAngleRadians(glm::vec3(0, 1, 0), eulerAngles.y);
	const glm::quat qz = fromAxisAngleRadians(glm::vec3(0, 0, 1), eulerAngles.z);
	return qz * qy * qx;
}

glm::quat
fromAxisAngleRadians(const glm::vec3& axis, float angle)
{
	const float c = cos(0.5 * angle);
	const float s = sin(0.5 * angle);
	const glm::vec3 n = glm::normalize(axis);
	return glm::quat(c, s * n.x, s * n.y, s * n.z);
}
