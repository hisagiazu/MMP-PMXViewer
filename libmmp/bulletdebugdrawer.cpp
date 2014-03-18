#include <GL/glew.h>
#include <GL/glfw.h>

#include "bulletdebugdrawer.h"

#include "shader.h"
#include "motioncontroller.h"

#include <iostream>

using namespace std;

#define BUFFER_OFFSET(offset) ((void *) (offset))

BulletDebugDrawer::BulletDebugDrawer(string vertexShaderPath, string fragmentShaderPath): m_debugMode(0)
{
	shaderProgram=compileShaders(vertexShaderPath,fragmentShaderPath);
	
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);
	
	glGenBuffers(1,&VertexArrayBuffer);
	glBindBuffer(GL_ARRAY_BUFFER,VertexArrayBuffer);
	
	//0-vPosition
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, BUFFER_OFFSET(0));
	glBindAttribLocation(shaderProgram, 0, "vPosition"); //Explicit vertex attribute index specification for older OpenGL version support. (Newer method is layout qualifier in vertex shader)
	glEnableVertexAttribArray(0);
	
	//1-vColor
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*8, BUFFER_OFFSET(sizeof(GLfloat)*4));
	glBindAttribLocation(shaderProgram, 1, "vColor");
	glEnableVertexAttribArray(1);
	
	linkShaders(shaderProgram);
	glUseProgram(shaderProgram);
	
	MVPLoc = glGetUniformLocation(shaderProgram, "MVP");
}

void BulletDebugDrawer::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
	lines.push_back(glm::vec4(from.getX(),from.getY(),from.getZ(),1.f));
	lines.push_back(glm::vec4(to.getX(),to.getY(),to.getZ(),1.f));
	
	lineColors.push_back(glm::vec4(color.getX(),color.getY(),color.getZ(),1.f));
	lineColors.push_back(glm::vec4(color.getX(),color.getY(),color.getZ(),1.f));
}

void BulletDebugDrawer::render()
{	
	glUseProgram(shaderProgram);
	glPointSize(5.0f);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER,VertexArrayBuffer);	
	
	float lineV[lines.size()][8];
	for(int i=0; i<lines.size(); ++i)
	{
		lineV[i][0]=lines[i].x;
		lineV[i][1]=lines[i].y;
		lineV[i][2]=lines[i].z;
		lineV[i][3]=lines[i].w;
		
		lineV[i][4]=lineColors[i].x;
		lineV[i][5]=lineColors[i].y;
		lineV[i][6]=lineColors[i].z;
		lineV[i][7]=lineColors[i].w;
	}
	
	glBufferData(GL_ARRAY_BUFFER, lines.size()*sizeof(GLfloat)*8, &lineV, GL_DYNAMIC_DRAW);
	
	glDrawArrays(GL_POINTS,0,lines.size());
	glDrawArrays(GL_LINES,0,lines.size());
		
	lines.clear();
	lineColors.clear();
	btDebugVertices.clear();
}

void BulletDebugDrawer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{
}
void BulletDebugDrawer::draw3dText(const btVector3 &location, const char *textString)
{
}

void BulletDebugDrawer::reportErrorWarning (const char *warningString)
{
	cout<<warningString<<endl;
}

void BulletDebugDrawer::setDebugMode (int debugMode)
{
	m_debugMode = debugMode;
}

int BulletDebugDrawer::getDebugMode() const
{
	return m_debugMode;
}
