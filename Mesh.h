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
class Mesh :
	public Object
{
private:
	static unsigned short totalResourceID;

public:
	Mesh();
	virtual ~Mesh();

public:
	//Type�� ���ϸ� �ȵ�. �׳� �޸𸮹��۸� �ְ�, ���� ���� Ȥ�� ��Ʈ���̵����� �ѱ�
	virtual void SetMeshData(VertexBuffer* vertexBuffer, std::vector<int>& indices);
	void DrawInstance(mat4x4* pMat, int count, GLenum drawMode = GL_TRIANGLES);

private:
	VertexBuffer* vertexBuffer;
	std::vector<int> indices;

	GLuint vertexArrayID;
	GLuint matrixBufferID;

	GetMacro(unsigned short, ResourceID, resourceID);
};