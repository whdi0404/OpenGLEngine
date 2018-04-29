#include "stdafx.h"
#include "Shader.h"

Shader::Shader(std::string vertexShaderPath, std::string pixelShaderPath, std::string tessCtrlShaderPath, std::string tessEvelShaderPath, std::string geomShaderPath)
{
	const char* c_str_tessCtrlShaderPath = nullptr;
	const char* c_str_tessEvelShaderPath = nullptr;
	const char* c_str_geomShaderPath = nullptr;
	if (!tessCtrlShaderPath.empty())
		c_str_tessCtrlShaderPath = tessCtrlShaderPath.c_str();
	if (!tessEvelShaderPath.empty())
		c_str_tessEvelShaderPath = tessEvelShaderPath.c_str();
	if (!geomShaderPath.empty())
		c_str_geomShaderPath = geomShaderPath.c_str();

	shaderID = LoadDefaultShaders(vertexShaderPath.c_str(), pixelShaderPath.c_str(), c_str_tessCtrlShaderPath, c_str_tessEvelShaderPath, c_str_geomShaderPath);
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