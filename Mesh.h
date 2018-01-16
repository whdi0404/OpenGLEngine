#pragma once
#include "Object.h"
#include "MeshVertexAttribute.h"
//struct DefaultVertex
//{
//	DefaultVertex(vec3 pos, vec2 uv) : position(pos), uv(uv)
//	{}
//	vec3 position;
//	vec2 uv;
//};

class VertexBuffer;
class Material;
class Mesh :
	public Object
{
private:
	static unsigned short totalResourceID;

public:
	Mesh();
	virtual ~Mesh();

public:
	virtual void SetMeshData(VertexBuffer* vertexBuffer, std::vector<int>& indices);
	virtual void DrawInstance(glm::mat4x4* pMat, int count);
	virtual void Draw();

protected:
	GetSetMacroProtected(VertexBuffer*, VertexBuffer, vertexBuffer);
	GetSetMacroProtected(std::vector<int>, Indices, indices);
	GLuint vertexArrayID;
	GLuint matrixBufferID;

	GetMacroProtected(unsigned short, ResourceID, resourceID);
	GetSetMacroProtected(GLenum, DrawMode, drawMode);
	GetMacro(Math::Sphere, BoundingSphere, boundingSphere);
};