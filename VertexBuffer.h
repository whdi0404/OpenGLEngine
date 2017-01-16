#pragma once
#include "MeshVertexAttribute.h"

class VertexBuffer
{
	GetMacro(MeshVertexAttribute*, Attribute, attribute);
	GetMacro(byte*, BufferPointer, buffer);
	GetMacro(int, VertexCount, vertexCount);
	GetMacro(int, BufferSize, bufferSize);
public:
	VertexBuffer(MeshVertexAttribute* attribute);
	~VertexBuffer();

	glm::vec4 GetVector(Element element, int index);
	void SetVector(Element element, int index, glm::vec4 vector);
	void SetVertexCount(int vertexCount);
};
