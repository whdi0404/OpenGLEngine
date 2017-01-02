#include "stdafx.h"
#include "Shader.h"

Shader::Shader(std::string vertexShaderPath, std::string pixelShaderPath, std::string tessCtrlShaderPath, std::string tessEvelShaderPath)
{
	const char* c_str_tessCtrlShaderPath = nullptr;
	const char* c_str_tessEvelShaderPath = nullptr;
	if (!tessCtrlShaderPath.empty())
		c_str_tessCtrlShaderPath = tessCtrlShaderPath.c_str();
	if (!tessEvelShaderPath.empty())
		c_str_tessEvelShaderPath = tessEvelShaderPath.c_str();
	shaderID = LoadDefaultShaders(vertexShaderPath.c_str(), pixelShaderPath.c_str(), c_str_tessCtrlShaderPath, c_str_tessEvelShaderPath);
}

Shader::Shader()
{
	shaderID = glCreateProgram();
}

Shader::~Shader()
{
}

void Shader::AttachShader(GLuint shaderID)
{
	AttachShaderToProgram(this->shaderID, 1, shaderID);
}