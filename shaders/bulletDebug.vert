#version 150

//Vertex Properties
in vec4 vPosition;
in vec4 vColor;

out vec4 fColor;

uniform mat4 MVP;

void main()
{
	gl_Position=MVP*vPosition; //MVP * vPosition
	fColor=vColor;
}
