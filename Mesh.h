#pragma once
#include "Object.h"
struct DefaultVertex
{
	DefaultVertex(vec3 pos, vec2 uv) : position(pos), uv(uv)
	{}
	vec3 position;
	vec2 uv;
};

class Mesh :
	public Object
{
private:
	static unsigned short totalResourceID;

public:
	static std::vector<Mesh*> LoadFBX(std::string filePath);

public:
	Mesh();
	~Mesh();

public:
	void SetVertices(std::vector<DefaultVertex>& vertices, std::vector<int>& indices);
	void DrawInstance(mat4x4* pMat, int count, GLenum drawMode = GL_TRIANGLES);

private:
	std::vector<DefaultVertex> vertices;
	std::vector<int> indices;

	GLuint vertexArrayID;

	//indexBuffer
	//AttributeBase* attr;

	//GLuint shaderProgramID;
	GLuint matrixBufferID;

	mat4* mat;
	Get(unsigned short, ResourceID, resourceID);
};

