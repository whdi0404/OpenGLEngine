#include "stdafx.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

unsigned short Material::totalResourceID = 0;

Material::Material(Shader* shader, unsigned short renderOrder) : shader(shader), renderOrder(renderOrder)
{
	resourceID = totalResourceID++;

	GLuint shaderID = shader->GetShaderID();
	glUseProgram(shaderID);
	matProj = glGetUniformLocation(shaderID, "matProj");
	matView = glGetUniformLocation(shaderID, "matView");
}

Material::~Material()
{
}
#define KEYVALUE(keyType, valueType) std::unordered_map<keyType, valueType>::value_type
void Material::SetTexture(std::string& paramName, Texture2D * texture)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
	{
		iter->second.pData = texture;
	}
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, texture)));
}

void Material::SetFloat(std::string& paramName, float value)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
	{
		*((float*)(iter->second.pData)) = value;
		iter->second = ParamData(this, paramName, value);
	}
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, value)));
}

void Material::SetVec2(std::string& paramName, vec2 vector)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
	{
		*((vec2*)(iter->second.pData)) = vector;
		iter->second = ParamData(this, paramName, vector);
	}
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, vector)));
}

void Material::SetVec3(std::string& paramName, vec3 vector)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
	{
		*((vec3*)(iter->second.pData)) = vector;
		iter->second = ParamData(this, paramName, vector);
	}
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, vector)));
}

void Material::SetVec4(std::string& paramName, vec4 vector)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
	{
		*((vec4*)(iter->second.pData)) = vector;
		iter->second = ParamData(this, paramName, vector);
	}
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, vector)));
}

void Material::SetMatrix2x2(std::string& paramName, mat2x2 matrix)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
	{
		*((mat2x2*)(iter->second.pData)) = matrix;
		iter->second = ParamData(this, paramName, matrix);
	}
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, matrix)));
}

void Material::SetMatrix3x3(std::string& paramName, mat3x3 matrix)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
	{
		*((mat3x3*)(iter->second.pData)) = matrix;
		iter->second = ParamData(this, paramName, matrix);
	}
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, matrix)));
}

void Material::SetMatrix4x4(std::string& paramName, mat4x4 matrix)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
	{
		*((mat4x4*)(iter->second.pData)) = matrix;
		iter->second = ParamData(this, paramName, matrix);
	}
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, matrix)));
}

void Material::BindUniformValue()
{
	int textureCount = 0;
	for (auto iter = hashMap_paramData.begin(); iter != hashMap_paramData.end(); ++iter)
	{
		iter->second.SetParam(textureCount);
		if (iter->second.GetType() == Material::ParamData::Type::TEX2D)
			++textureCount;
	}
}

void Material::BindShader()
{
	glUseProgram(shader->GetShaderID());
}

void Material::SetCameraMatrix(Camera* cam)
{
	//mat4x4 mat = mat4x4();
	glUniformMatrix4fv(matProj, 1, GL_FALSE, &cam->GetMatProj()[0][0]);
	glUniformMatrix4fv(matView, 1, GL_FALSE, &cam->GetMatView()[0][0]);
}

Material::ParamData::ParamData(Material * mtrl, std::string& paramName, Texture2D*& texture2D)
{
	type = TEX2D;
	pData = texture2D;

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string& paramName, float & value)
{
	type = FLOAT;
	pData = new float(value);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string& paramName, vec2 & vector2)
{
	type = VEC2;
	pData = new vec2(vector2);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string& paramName, vec3 & vector3)
{
	type = VEC3;
	pData = new vec3(vector3);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string& paramName, vec4 & vector4)
{
	type = VEC4;
	pData = new vec4(vector4);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string& paramName, mat2x2 & mat2)
{
	type = MAT2X2;
	pData = new mat2x2(mat2);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string& paramName, mat3x3 & mat3)
{
	type = MAT3X3;
	pData = new mat3x3(mat3);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string& paramName, mat4x4 & mat4)
{
	type = MAT4X4;
	pData = new mat4x4(mat4);

	BindUniformValue(mtrl, paramName);
}

void Material::ParamData::ReleaseData()
{
	if (pData == nullptr) return;
	switch (type)
	{
		case Type::TEX2D:
			break;
		case Type::FLOAT:
			delete (float*)pData;
			break;
		case Type::VEC2:
			delete (vec2*)pData;
			break;
		case Type::VEC3:
			delete (vec3*)pData;
			break;
		case Type::VEC4:
			delete (vec4*)pData;
			break;
		case Type::MAT2X2:
			delete (mat2x2*)pData;
			break;
		case Type::MAT3X3:
			delete (mat3x3*)pData;
			break;
		case Type::MAT4X4:
			delete (mat4x4*)pData;
			break;
	}
	pData = nullptr;
}

void Material::ParamData::SetParam(int arg1)
{
	switch (type)
	{
	case Type::FLOAT:
		glUniform1f(uniformLocation, *(float*)pData);
		break;
	case Type::TEX2D:
		glUniform1i(uniformLocation, arg1);
		glActiveTexture(GL_TEXTURE0 + arg1);
		glBindTexture(GL_TEXTURE_2D, ((Texture2D*)pData)->GetTextureID());
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
		break;
	case Type::VEC2:
	{
		vec2* val = (vec2*)pData;
		glUniform2f(uniformLocation, val->x, val->y);
	}
		break;
	case Type::VEC3:
	{
		vec3* val = (vec3*)pData;
		glUniform3f(uniformLocation, val->x, val->y, val->z);
	}
		break;
	case Type::VEC4:
	{
		vec4* val = (vec4*)pData;
		glUniform4f(uniformLocation, val->x, val->y, val->z, val->w);
	}
		break;
	case Type::MAT2X2:
		glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, (GLfloat*)pData);
		break;
	case Type::MAT3X3:
		glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, (GLfloat*)pData);
		break;
	case Type::MAT4X4:
		glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, (GLfloat*)pData);
		break;
	}
}

void Material::ParamData::BindUniformValue(Material* mtrl, std::string& paramName)
{
	GLuint shaderID = mtrl->shader->GetShaderID();
	glUseProgram(shaderID);
	uniformLocation = glGetUniformLocation(shaderID, paramName.c_str());
}
