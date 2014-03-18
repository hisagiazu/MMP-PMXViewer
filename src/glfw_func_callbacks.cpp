#include "glfw_func_callbacks.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//Spherical coordinate system (http://en.wikipedia.org/wiki/Spherical_coordinate_system)
float theta=3.0f*M_PI/2.0f;
float zenith=0.0;
float radius=10.0f;
float t=radius; //distances to center after rotating up/down

float theta2=0.0f;
float radius2=20.0f;
glm::vec3 cameraPosition(0.0f, 0.0f, radius*sin(theta));
glm::vec3 cameraTarget(0.0f,0.0f,0.0f);

int prevX=0,prevY=0;
int prevWheel=0;


void GLFWCALL mouseWheel(int pos)
{
	int dWheel=pos-prevWheel;
	if(dWheel>0) //up
	{
		radius-=1.0f;
		t=radius*cos(zenith);
		cameraPosition.y=radius*sin(zenith);
	
		cameraPosition.x=t*cos(theta);
		cameraPosition.z=t*sin(theta);
	}
	else if(dWheel<0) //down
	{
		radius+=1.0f;
		t=radius*cos(zenith);
		cameraPosition.y=radius*sin(zenith);
	
		cameraPosition.x=t*cos(theta);
		cameraPosition.z=t*sin(theta);
	}
	
	prevWheel=pos;
}

void GLFWCALL mouseMotion(int x, int y)
{
	int dx=prevX-x,dy=y-prevY;

	if(glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
	{
		theta+=dx/250.0f;
		zenith+=dy/250.0f;
	
		if(zenith>=M_PI/2) zenith=M_PI/2-0.0001f;
		if(zenith<=-M_PI/2) zenith=-M_PI/2+0.0001f;
	
		//cout<<theta<<" "<<zenith<<endl;
		//cout<<dx<<" "<<dy<<endl;
	
		t=radius*cos(zenith);
		cameraPosition.y=radius*sin(zenith);
	
		cameraPosition.x=t*cos(theta);
		cameraPosition.z=t*sin(theta);
	
	
		//cameraPosition.z=radius*sin(theta)+radius2*cos(theta2);
	
	}
	prevX=x, prevY=y;
}

