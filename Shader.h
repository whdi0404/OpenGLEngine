#pragma once
#include "Object.h"

class Shader : Object
{
public:
	Shader(std::string vertexShaderPath, std::string pixelShaderPath, std::string tessCtrlShaderPath = "", std::string tessEvelShaderPath = "");
	Shader();
	~Shader();

	void AttachShader(GLuint shaderID);
	GetMacro(GLuint, ShaderID, shaderID);
};