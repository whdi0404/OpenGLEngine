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

	struct ParamData
	{
		enum Type {TEX2D, FLOAT, VEC2, VEC3, VEC4, MAT2X2, MAT3X3, MAT4X4};
		
		ParamData(Material* mtrl, std::string& paramName, Texture2D*& texture2D);
		ParamData(Material* mtrl, std::string& paramName, float& value);
		ParamData(Material* mtrl, std::string& paramName, vec2& vector2);
		ParamData(Material* mtrl, std::string& paramName, vec3& vector3);
		ParamData(Material* mtrl, std::string& paramName, vec4& vector4);
		ParamData(Material* mtrl, std::string& paramName, mat2x2& mat2);
		ParamData(Material* mtrl, std::string& paramName, mat3x3& mat3);
		ParamData(Material* mtrl, std::string& paramName, mat4x4& mat4);
		
		void ReleaseData();
		void SetParam(int arg1);
		void BindUniformValue(Material* mtrl, std::string& paramName);

	private:
		int TextureValue;
		GetMacro(Type, Type, type);
		GLuint uniformLocation;
		void* pData;
	};

public:
	Material(Shader* shader, unsigned short renderOrder);
	~Material();

	void SetTexture(std::string& paramName, Texture2D* texture);
	void SetFloat(std::string& paramName, float value);
	void SetVec2(std::string& paramName, vec2 vector);
	void SetVec3(std::string& paramName, vec3 vector);
	void SetVec4(std::string& paramName, vec4 vector);
	void SetMatrix2x2(std::string& paramName, mat2x2 matrix);
	void SetMatrix3x3(std::string& paramName, mat3x3 matrix);
	void SetMatrix4x4(std::string& paramName, mat4x4 matrix);

	void BindUniformValue();
	void BindShader();
	void SetCameraMatrix(Camera* cam);

private:
	std::unordered_map <std::string, ParamData> hashMap_paramData;
	GLuint matProj;
	GLuint matView;
	Shader* shader;

	GetMacro(unsigned short, ResourceID, resourceID);
	GetMacro(unsigned short, RenderOrder, renderOrder);
};