#ifndef SHADER_H
#define SHADER_H

#include <iostream>



GLuint compileShaders(std::string vertShaderName, std::string fragShaderName);
void linkShaders(GLuint program);

#endif
