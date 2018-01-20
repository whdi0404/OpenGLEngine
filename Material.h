#pragma once
#include "Object.h"

class Texture2D;
class Shader;
class Camera;
class Material :
	public Object
{
private:
	static unsigned short totalResourceID;

	struct UBOData
	{
		enum Type {TEX2D, INT, FLOAT, VEC2, VEC3, VEC4, MAT2X2, MAT3X3, MAT4X4};

		static void* GetBuffer(Type type, int count);
		
		UBOData(Material* mtrl, std::string& paramName, Texture2D* texture2D);
		UBOData(Material* mtrl, std::string& paramName, int* value, int count);
		UBOData(Material* mtrl, std::string& paramName, float* value, int count);
		UBOData(Material* mtrl, std::string& paramName, glm::vec2* vector2, int count);
		UBOData(Material* mtrl, std::string& paramName, glm::vec3* vector3, int count);
		UBOData(Material* mtrl, std::string& paramName, glm::vec4* vector4, int count);
		UBOData(Material* mtrl, std::string& paramName, glm::mat2x2* mat2, int count);
		UBOData(Material* mtrl, std::string& paramName, glm::mat3x3* mat3, int count);
		UBOData(Material* mtrl, std::string& paramName, glm::mat4x4* mat4, int count);

		void ReleaseData();
		void SetData(void* data, int dataSize);
		void SetUniformParam(int activeTexture);
		void BindUniformValue(Material* mtrl, std::string& paramName);

		GetMacro(Type, Type, type);
	private:
		int TextureValue;
		GLuint uniformId;
		void* pData;
		int count;
	};

	struct SSBOData
	{
		SSBOData(Material* mtrl, GLuint locationId, void* data, int size);

		void SetData(void* data, int dataSize);
		void ReleaseData();
		void SetSSBOParam();
	private:
		GLuint locationId;
		GLuint ssboId;
		void* pData;
		int size;
	};

public:
	Material(Shader* shader, unsigned short renderOrder);
	~Material();

	void SetTexture(std::string& paramName, Texture2D* texture);
	void SetInt(std::string& paramName, int* value, int count);
	void SetFloat(std::string& paramName, float* value, int count);
	void SetVec2(std::string& paramName, glm::vec2* vector, int count);
	void SetVec3(std::string& paramName, glm::vec3* vector, int count);
	void SetVec4(std::string& paramName, glm::vec4* vector, int count);
	void SetMatrix2x2(std::string& paramName, glm::mat2x2* matrix, int count);
	void SetMatrix3x3(std::string& paramName, glm::mat3x3* matrix, int count);
	void SetMatrix4x4(std::string& paramName, glm::mat4x4* matrix, int count);
	void SetSSBOData(GLuint location, void* data, int size);

	void BindShader();
	void BindBuffer();
	void SetCameraMatrix(Camera* cam);
private:
	void BindUniformValue();
	void BindSSBOValue();

private:
	std::unordered_map <std::string, UBOData> hashMap_uniformData;
	std::unordered_map <GLuint, SSBOData> hashMap_SSBOData;
	GLuint matProj;
	GLuint matView;
	Shader* shader;

	GetMacro(unsigned short, ResourceID, resourceID);
	GetMacro(unsigned short, RenderOrder, renderOrder);
};