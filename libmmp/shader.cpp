#include <GL/glew.h>
#include <OpenGL/gl.h>

#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

const GLchar* ReadShader(const char* filename)
{
#ifdef _MSVC
	FILE* infile;
	fopen_s( &infile, filename, "rb" );
#else
    FILE* infile = fopen( filename, "rb" );
#endif //_MSVC

    if ( !infile ) {
#ifdef _DEBUG
        std::cerr << "Unable to open file '" << filename << "'" << std::endl;
#endif /* DEBUG */
        return NULL;
    }

    fseek( infile, 0, SEEK_END );
    int len = ftell( infile );
    fseek( infile, 0, SEEK_SET );

    GLchar* source = new GLchar[len+1];

    fread( source, 1, len, infile );
    fclose( infile );

    source[len] = 0;

    return const_cast<const GLchar*>(source);
}

GLuint compileShader(GLenum type, const GLchar *filename)
{
	cout<<"Compiling "<<filename<<"...";
	
	const GLchar* source=ReadShader(filename);
	if(source==NULL)
	{
		cerr<<"FATAL ERROR: shader file could not be read"<<endl;
		exit(EXIT_FAILURE);
	}
	
	//cout<<"Source: "<<source<<endl;
	
	GLuint shader=glCreateShader(type);
	
	glShaderSource(shader, 1, &source, NULL);
	
	glCompileShader(shader);
	GLint compileStatus=-1;
	glGetShaderiv(shader,  GL_COMPILE_STATUS,  &compileStatus);
	//cout<<"Compile Status: "<<compileStatus<<endl;
	if(compileStatus==GL_FALSE)
	{
		char *shaderLog=(char*) malloc(sizeof(char)*500);
		glGetShaderInfoLog(shader, sizeof(char)*500, NULL, shaderLog);
		
		cout<<endl<<shaderLog<<endl;
		
		exit(EXIT_FAILURE);
	}
	
	cout<<"done"<<endl;
	
	return shader;
}


GLuint compileShaders(string vertShaderName, string fragShaderName)
{
	glewExperimental = GL_TRUE;
	glewInit();
	
	GLuint vertShader=compileShader(GL_VERTEX_SHADER,vertShaderName.c_str());
	GLuint fragShader=compileShader(GL_FRAGMENT_SHADER,fragShaderName.c_str());

	GLuint program=glCreateProgram();
	glAttachShader(program,vertShader);
	glAttachShader(program,fragShader);
	
	return program;
}

void linkShaders(GLuint program)
{
	cout<<"Linking shaders...";
	glLinkProgram(program);
	
	GLint linkStatus=-1;
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	//cout<<"Link Status: "<<linkStatus<<endl;
	
	if(linkStatus==GL_FALSE)
	{
		char *linkerLog=(char*) malloc(sizeof(char)*500);
		glGetProgramInfoLog(program, sizeof(char)*500, NULL, linkerLog);
		
		cout<<endl<<linkerLog<<endl;
		
		exit(EXIT_FAILURE);
	}
	
	cout<<"done"<<endl;
}
