#if !defined(LIBMMP_GLM_HELPER_H)
#define LIBMMP_GLM_HELPER_H

#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>
#include <iosfwd>

std::ostream& operator<<(std::ostream&, const glm::vec3&);
std::ostream& operator<<(std::ostream&, const glm::vec4&);
std::ostream& operator<<(std::ostream&, const glm::quat&);
std::ostream& operator<<(std::ostream&, const glm::mat4&);

//! mutual conversion of lower/upper limit in euler angles between Direct3D and OpenGL
void flipZAxisOfRotationalLimits(glm::vec3& rLower, glm::vec3& rUpper);

//! mutual conversion between Direct3D and OpenGL
glm::vec3 flipZAxisOfEulerAnglesRadians(const glm::vec3&);

//! mutual conversion between Direct3D and OpenGL
glm::quat flipZAxisOfQuaternion(const glm::quat&);

//! Conversion from quaternion to euler angles (XYZ euler angles in radians)
glm::vec3 toEulerAnglesRadians(const glm::quat&);

//! Conversion from euler angles (XYZ euler angles in radians) to quaternion
glm::quat fromEulerAnglesRadians(const glm::vec3&);

//! Construct quaternion from an axis vector and an angle.
//!
//! An angle has to be expressed in radian.
//! An axis vector doesn't have to be normalized.
glm::quat fromAxisAngleRadians(const glm::vec3& axis, float angle);

#endif /* LIBMMP_GLM_HELPER_H */
