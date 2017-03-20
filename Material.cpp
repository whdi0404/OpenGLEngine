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
	glUseProgram(0);
}

Material::~Material()
{
}

void Material::SetTexture(std::string& paramName, Texture2D * texture)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
		iter->second.SetData(texture, 1);
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, texture)));
}

void Material::SetFloat(std::string& paramName, float* value, int count)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
		iter->second.SetData(value, count);
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, value, count)));
}

void Material::SetVec2(std::string& paramName, glm::vec2* vector, int count)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
		iter->second.SetData(vector, count);
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, vector, count)));
}

void Material::SetVec3(std::string& paramName, glm::vec3* vector, int count)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
		iter->second.SetData(vector, count);
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, vector, count)));
}

void Material::SetVec4(std::string& paramName, glm::vec4* vector, int count)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
		iter->second.SetData(vector, count);
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, vector, count)));
}

void Material::SetMatrix2x2(std::string& paramName, glm::mat2x2* matrix, int count)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
		iter->second.SetData(matrix, count);
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, matrix, count)));
}

void Material::SetMatrix3x3(std::string& paramName, glm::mat3x3* matrix, int count)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
		iter->second.SetData(matrix, count);
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, matrix, count)));
}

void Material::SetMatrix4x4(std::string& paramName, glm::mat4x4* matrix, int count)
{
	auto iter = hashMap_paramData.find(paramName);
	if (iter != hashMap_paramData.end())
		iter->second.SetData(matrix, count);
	else
		hashMap_paramData.insert(KEYVALUE(std::string, ParamData)(paramName, ParamData(this, paramName, matrix, count)));
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
	//glm::mat4x4 mat = glm::mat4x4();
	glUniformMatrix4fv(matProj, 1, GL_FALSE, &cam->GetMatProj()[0][0]);
	glUniformMatrix4fv(matView, 1, GL_FALSE, &cam->GetMatView()[0][0]);
}

void* Material::ParamData::GetBuffer(Type type, int count)
{
	switch (type)
	{
	case Type::FLOAT:
		return new float[count];
	case Type::VEC2:
		return new glm::vec2[count];
	case Type::VEC3:
		return new glm::vec3[count];
	case Type::VEC4:
		return new glm::vec4[count];
	case Type::MAT2X2:
		return new glm::mat2x2[count];
	case Type::MAT3X3:
		return new glm::mat3x3[count];
	case Type::MAT4X4:
		return new glm::mat4x4[count];
	}
	return nullptr;
}

Material::ParamData::ParamData(Material * mtrl, std::string& paramName, Texture2D* texture2D) : count(1)
{
	type = TEX2D;
	pData = texture2D;

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string & paramName, float * value, int count) : count(count)
{
	type = FLOAT;
	pData = new float[count];
	memcpy_s(pData, sizeof(float) * count, value, sizeof(float) * count);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string & paramName, glm::vec2 * vector2, int count) : count(count)
{
	type = VEC2;
	pData = new glm::vec2[count];
	memcpy_s(pData, sizeof(glm::vec2) * count, vector2, sizeof(glm::vec2) * count);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string & paramName, glm::vec3 * vector3, int count) : count(count)
{
	type = VEC3;
	pData = new glm::vec3[count];
	memcpy_s(pData, sizeof(glm::vec3) * count, vector3, sizeof(glm::vec3) * count);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string & paramName, glm::vec4 * vector4, int count) : count(count)
{
	type = VEC4;
	pData = new glm::vec4[count];
	memcpy_s(pData, sizeof(glm::vec4) * count, vector4, sizeof(glm::vec4) * count);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string & paramName, glm::mat2x2 * mat2, int count) : count(count)
{
	type = MAT2X2;
	pData = new glm::mat2x2[count];
	memcpy_s(pData, sizeof(glm::mat2x2) * count, mat2, sizeof(glm::mat2x2) * count);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string & paramName, glm::mat3x3 * mat3, int count) : count(count)
{
	type = MAT3X3;
	pData = new glm::mat3x3[count];
	memcpy_s(pData, sizeof(glm::mat3x3) * count, mat3, sizeof(glm::mat3x3) * count);

	BindUniformValue(mtrl, paramName);
}

Material::ParamData::ParamData(Material * mtrl, std::string & paramName, glm::mat4x4 * mat4, int count) : count(count)
{
	type = MAT4X4;
	pData = new glm::mat4x4[count];
	memcpy_s(pData, sizeof(glm::mat4x4) * count, mat4, sizeof(glm::mat4x4) * count);

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
			delete[](float*)pData;
			break;
		case Type::VEC2:
			delete[](glm::vec2*)pData;
			break;
		case Type::VEC3:
			delete[](glm::vec3*)pData;
			break;
		case Type::VEC4:
			delete[](glm::vec4*)pData;
			break;
		case Type::MAT2X2:
			delete[](glm::mat2x2*)pData;
			break;
		case Type::MAT3X3:
			delete[](glm::mat3x3*)pData;
			break;
		case Type::MAT4X4:
			delete[](glm::mat4x4*)pData;
			break;
	}
	pData = nullptr;
}

void Material::ParamData::SetData(void * data, int count)
{
	if (this->count != count)
	{
		ReleaseData();
		this->count = count;
		pData = GetBuffer(type, count);
	}
	
	switch (type)
	{
	case Type::TEX2D:
		pData = data;
	case Type::FLOAT:
		for (int i = 0; i < count; ++i)
			((float*)pData)[i] = ((float*)data)[i];
		break;
	case Type::VEC2:
		for (int i = 0; i < count; ++i)
			((glm::vec2*)pData)[i] = ((glm::vec2*)data)[i];
		break;
	case Type::VEC3:
		for (int i = 0; i < count; ++i)
			((glm::vec3*)pData)[i] = ((glm::vec3*)data)[i];
		break;
	case Type::VEC4:
		for (int i = 0; i < count; ++i)
			((glm::vec4*)pData)[i] = ((glm::vec4*)data)[i];
		break;
	case Type::MAT2X2:
		for (int i = 0; i < count; ++i)
			((glm::mat2x2*)pData)[i] = ((glm::mat2x2*)data)[i];
		break;
	case Type::MAT3X3:
		for (int i = 0; i < count; ++i)
			((glm::mat3x3*)pData)[i] = ((glm::mat3x3*)data)[i];
		break;
	case Type::MAT4X4:
		for (int i = 0; i < count; ++i)
			((glm::mat4x4*)pData)[i] = ((glm::mat4x4*)data)[i];
		break;
	}
}

void Material::ParamData::SetParam(int activeTexture)
{
	switch (type)
	{
	case Type::FLOAT:
		glUniform1fv(uniformLocation, count, (GLfloat*)pData);
		break;
	case Type::TEX2D:
		glUniform1i(uniformLocation, activeTexture);
		glActiveTexture(GL_TEXTURE0 + activeTexture);
		glBindTexture(GL_TEXTURE_2D, ((Texture2D*)pData)->GetTextureID());
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
		break;
	case Type::VEC2:
		glUniform2fv(uniformLocation, count, (GLfloat*)pData);
		break;
	case Type::VEC3:
		glUniform3fv(uniformLocation, count, (GLfloat*)pData);
		break;
	case Type::VEC4:
		glUniform4fv(uniformLocation, count, (GLfloat*)pData);
		break;
	case Type::MAT2X2:
		glUniformMatrix2fv(uniformLocation, count, GL_FALSE, (GLfloat*)pData);
		break;
	case Type::MAT3X3:
		glUniformMatrix3fv(uniformLocation, count, GL_FALSE, (GLfloat*)pData);
		break;
	case Type::MAT4X4:
		glUniformMatrix4fv(uniformLocation, count, GL_FALSE, (GLfloat*)pData);
		break;
	}
}

void Material::ParamData::BindUniformValue(Material* mtrl, std::string& paramName)
{
	GLuint shaderID = mtrl->shader->GetShaderID();
	glUseProgram(shaderID);
	uniformLocation = glGetUniformLocation(shaderID, paramName.c_str());
}
