#ifndef GLFW_FUNC_CALLBACKS
#define GLFW_FUNC_CALLBACKS

#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GL/glfw.h>

#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

extern float theta;
extern float zenith;
extern float radius;
extern float t; //distances to center after rotating up/down

extern float theta2;
extern float radius2;
extern glm::vec3 cameraPosition;
extern glm::vec3 cameraTarget;

extern int prevX,prevY;
extern int prevWheel;

//***Mouse Variables***
void GLFWCALL mouseWheel(int pos);
void GLFWCALL mouseMotion(int x, int y);
	


#endif
