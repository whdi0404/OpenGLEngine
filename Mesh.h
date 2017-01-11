#pragma once
#include "Object.h"
struct DefaultVertex
{
	DefaultVertex(vec3 pos, vec2 uv) : position(pos), uv(uv)
	{}
	vec3 position;
	vec2 uv;
};

struct SkinnedVertex
{

};

class Mesh :
	public Object
{
private:
	static unsigned short totalResourceID;

public:
	static std::vector<Object*> LoadMeshes(std::string filePath);
	Object* GetMeshes(FbxNode* node);

public:
	Mesh();
	~Mesh();

public:
	//Type을 정하면 안됨. 그냥 메모리버퍼만 주고, 정점 갯수 혹은 스트라이드사이즈만 넘김
	void SetVertices(std::vector<DefaultVertex>& vertices, std::vector<int>& indices);
	void DrawInstance(mat4x4* pMat, int count, GLenum drawMode = GL_TRIANGLES);

private:
	//벡터 대신 메모리버퍼로 대체
	void* vertexBuffer;
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