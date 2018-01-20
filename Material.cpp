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
	auto iter = hashMap_uniformData.find(paramName);
	if (iter != hashMap_uniformData.end())
		iter->second.SetData(texture, 1);
	else
		hashMap_uniformData.insert(KEYVALUE(std::string, UBOData)(paramName, UBOData(this, paramName, texture)));
}

void Material::SetInt(std::string & paramName, int * value, int count)
{
	auto iter = hashMap_uniformData.find(paramName);
	if (iter != hashMap_uniformData.end())
		iter->second.SetData(value, count);
	else
		hashMap_uniformData.insert(KEYVALUE(std::string, UBOData)(paramName, UBOData(this, paramName, value, count)));
}

void Material::SetFloat(std::string& paramName, float* value, int count)
{
	auto iter = hashMap_uniformData.find(paramName);
	if (iter != hashMap_uniformData.end())
		iter->second.SetData(value, count);
	else
		hashMap_uniformData.insert(KEYVALUE(std::string, UBOData)(paramName, UBOData(this, paramName, value, count)));
}

void Material::SetVec2(std::string& paramName, glm::vec2* vector, int count)
{
	auto iter = hashMap_uniformData.find(paramName);
	if (iter != hashMap_uniformData.end())
		iter->second.SetData(vector, count);
	else
		hashMap_uniformData.insert(KEYVALUE(std::string, UBOData)(paramName, UBOData(this, paramName, vector, count)));
}

void Material::SetVec3(std::string& paramName, glm::vec3* vector, int count)
{
	auto iter = hashMap_uniformData.find(paramName);
	if (iter != hashMap_uniformData.end())
		iter->second.SetData(vector, count);
	else
		hashMap_uniformData.insert(KEYVALUE(std::string, UBOData)(paramName, UBOData(this, paramName, vector, count)));
}

void Material::SetVec4(std::string& paramName, glm::vec4* vector, int count)
{
	auto iter = hashMap_uniformData.find(paramName);
	if (iter != hashMap_uniformData.end())
		iter->second.SetData(vector, count);
	else
		hashMap_uniformData.insert(KEYVALUE(std::string, UBOData)(paramName, UBOData(this, paramName, vector, count)));
}

void Material::SetMatrix2x2(std::string& paramName, glm::mat2x2* matrix, int count)
{
	auto iter = hashMap_uniformData.find(paramName);
	if (iter != hashMap_uniformData.end())
		iter->second.SetData(matrix, count);
	else
		hashMap_uniformData.insert(KEYVALUE(std::string, UBOData)(paramName, UBOData(this, paramName, matrix, count)));
}

void Material::SetMatrix3x3(std::string& paramName, glm::mat3x3* matrix, int count)
{
	auto iter = hashMap_uniformData.find(paramName);
	if (iter != hashMap_uniformData.end())
		iter->second.SetData(matrix, count);
	else
		hashMap_uniformData.insert(KEYVALUE(std::string, UBOData)(paramName, UBOData(this, paramName, matrix, count)));
}

void Material::SetMatrix4x4(std::string& paramName, glm::mat4x4* matrix, int count)
{
	auto iter = hashMap_uniformData.find(paramName);
	if (iter != hashMap_uniformData.end())
		iter->second.SetData(matrix, count);
	else
		hashMap_uniformData.insert(KEYVALUE(std::string, UBOData)(paramName, UBOData(this, paramName, matrix, count)));
}

void Material::SetSSBOData(GLuint location, void * data, int size)
{
	auto iter = hashMap_SSBOData.find(location);
	if (iter != hashMap_SSBOData.end())
		iter->second.SetData(data, size);
	else
		hashMap_SSBOData.insert(KEYVALUE(GLuint, SSBOData)(location, SSBOData(this, location, data, size)));
}

void Material::BindUniformValue()
{
	int textureCount = 0;
	for (auto iter = hashMap_uniformData.begin(); iter != hashMap_uniformData.end(); ++iter)
	{
		iter->second.SetUniformParam(textureCount);
		if (iter->second.GetType() == Material::UBOData::Type::TEX2D)
			++textureCount;
	}
}

void Material::BindSSBOValue()
{
	for (auto iter = hashMap_SSBOData.begin(); iter != hashMap_SSBOData.end(); ++iter)
	{
		iter->second.SetSSBOParam();
	}
}

void Material::BindShader()
{
	glUseProgram(shader->GetShaderID());
	BindBuffer();
}

void Material::BindBuffer()
{
	BindUniformValue();
	BindSSBOValue();
}

void Material::SetCameraMatrix(Camera* cam)
{
	glUniformMatrix4fv(matProj, 1, GL_FALSE, &cam->GetMatProj()[0][0]);
	glUniformMatrix4fv(matView, 1, GL_FALSE, &cam->GetMatView()[0][0]);
}

void* Material::UBOData::GetBuffer(Type type, int count)
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

Material::UBOData::UBOData(Material * mtrl, std::string& paramName, Texture2D* texture2D) : count(1)
{
	type = TEX2D;
	pData = texture2D;

	BindUniformValue(mtrl, paramName);
}

Material::UBOData::UBOData(Material * mtrl, std::string & paramName, int * value, int count) : count(count)
{
	type = INT;
	pData = new int[count];
	memcpy_s(pData, sizeof(int) * count, value, sizeof(int) * count);

	BindUniformValue(mtrl, paramName);
}

Material::UBOData::UBOData(Material * mtrl, std::string & paramName, float * value, int count) : count(count)
{
	type = FLOAT;
	pData = new float[count];
	memcpy_s(pData, sizeof(float) * count, value, sizeof(float) * count);

	BindUniformValue(mtrl, paramName);
}

Material::UBOData::UBOData(Material * mtrl, std::string & paramName, glm::vec2 * vector2, int count) : count(count)
{
	type = VEC2;
	pData = new glm::vec2[count];
	memcpy_s(pData, sizeof(glm::vec2) * count, vector2, sizeof(glm::vec2) * count);

	BindUniformValue(mtrl, paramName);
}

Material::UBOData::UBOData(Material * mtrl, std::string & paramName, glm::vec3 * vector3, int count) : count(count)
{
	type = VEC3;
	pData = new glm::vec3[count];
	memcpy_s(pData, sizeof(glm::vec3) * count, vector3, sizeof(glm::vec3) * count);

	BindUniformValue(mtrl, paramName);
}

Material::UBOData::UBOData(Material * mtrl, std::string & paramName, glm::vec4 * vector4, int count) : count(count)
{
	type = VEC4;
	pData = new glm::vec4[count];
	memcpy_s(pData, sizeof(glm::vec4) * count, vector4, sizeof(glm::vec4) * count);

	BindUniformValue(mtrl, paramName);
}

Material::UBOData::UBOData(Material * mtrl, std::string & paramName, glm::mat2x2 * mat2, int count) : count(count)
{
	type = MAT2X2;
	pData = new glm::mat2x2[count];
	memcpy_s(pData, sizeof(glm::mat2x2) * count, mat2, sizeof(glm::mat2x2) * count);

	BindUniformValue(mtrl, paramName);
}

Material::UBOData::UBOData(Material * mtrl, std::string & paramName, glm::mat3x3 * mat3, int count) : count(count)
{
	type = MAT3X3;
	pData = new glm::mat3x3[count];
	memcpy_s(pData, sizeof(glm::mat3x3) * count, mat3, sizeof(glm::mat3x3) * count);

	BindUniformValue(mtrl, paramName);
}

Material::UBOData::UBOData(Material * mtrl, std::string & paramName, glm::mat4x4 * mat4, int count) : count(count)
{
	type = MAT4X4;
	pData = new glm::mat4x4[count];
	memcpy_s(pData, sizeof(glm::mat4x4) * count, mat4, sizeof(glm::mat4x4) * count);

	BindUniformValue(mtrl, paramName);
}

void Material::UBOData::ReleaseData()
{
	if (pData == nullptr) return;
	switch (type)
	{
	case Type::TEX2D:
		break;
	case Type::INT:
		delete[](int*)pData;
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

void Material::UBOData::SetData(void * data, int count)
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
	case Type::INT:
		for (int i = 0; i < count; ++i)
			((int*)pData)[i] = ((int*)data)[i];
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

void Material::UBOData::SetUniformParam(int activeTexture)
{
	switch (type)
	{
	case Type::TEX2D:
	{
		glUniform1i(uniformId, activeTexture);
		glActiveTexture(GL_TEXTURE0 + activeTexture);

		Texture2D* pTex = (Texture2D*)pData;

		glBindTexture(GL_TEXTURE_2D, pTex->GetTextureID());
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pTex->GetMagFilter());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pTex->GetMinFilter());
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, pTex->GetAniso());

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	break;
	case Type::FLOAT:
		glUniform1fv(uniformId, count, (GLfloat*)pData);
		break;
	case Type::INT:
		glUniform1iv(uniformId, count, (GLint*)pData);
		break;
	case Type::VEC2:
		glUniform2fv(uniformId, count, (GLfloat*)pData);
		break;
	case Type::VEC3:
		glUniform3fv(uniformId, count, (GLfloat*)pData);
		break;
	case Type::VEC4:
		glUniform4fv(uniformId, count, (GLfloat*)pData);
		break;
	case Type::MAT2X2:
		glUniformMatrix2fv(uniformId, count, GL_FALSE, (GLfloat*)pData);
		break;
	case Type::MAT3X3:
		glUniformMatrix3fv(uniformId, count, GL_FALSE, (GLfloat*)pData);
		break;
	case Type::MAT4X4:
		glUniformMatrix4fv(uniformId, count, GL_FALSE, (GLfloat*)pData);
		break;
	}
}

void Material::UBOData::BindUniformValue(Material* mtrl, std::string& paramName)
{
	GLuint shaderID = mtrl->shader->GetShaderID();
	glUseProgram(shaderID);
	uniformId = glGetUniformLocation(shaderID, paramName.c_str());
}

Material::SSBOData::SSBOData(Material * mtrl, GLuint locationId, void* data, int dataSize) : pData(nullptr)
{
	glGenBuffers(1, &ssboId);
	this->locationId = locationId;
	SetData(data, dataSize);
}

void Material::SSBOData::SetData(void * data, int dataSize)
{
	if (pData != nullptr)
	{
		ReleaseData();
	}
	size = dataSize;
	pData = new byte[dataSize];
	memcpy_s(pData, size, data, dataSize);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboId);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, locationId, ssboId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, size, pData, GL_DYNAMIC_READ);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void Material::SSBOData::ReleaseData()
{
	delete[] (byte*)pData;
}

void Material::SSBOData::SetSSBOParam()
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboId);
}
