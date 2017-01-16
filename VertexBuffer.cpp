#include "stdafx.h"
#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(MeshVertexAttribute* attribute) : attribute(attribute), buffer(nullptr), bufferSize(0), vertexCount(0)
{
}

VertexBuffer::~VertexBuffer()
{
	delete buffer;
}

glm::vec4 VertexBuffer::GetVector(Element element, int index)
{
	if (index >= bufferSize || attribute->HasElement(element) == false)
		return glm::vec4(0, 0, 0, 0);

	byte* startOffset = buffer + index * attribute->GetVertexSize() + attribute->GetElementStartOffset(element);
	glm::vec4 vector;
	memcpy(&vector, startOffset, attribute->GetElementSize(element));

	return vector;
}

void VertexBuffer::SetVector(Element element, int index, glm::vec4 vector)
{
	if (index >= bufferSize || attribute->HasElement(element) == false)
		return;

	byte* startOffset = buffer + index * attribute->GetVertexSize() + attribute->GetElementStartOffset(element);
	memcpy(startOffset, &vector, attribute->GetElementSize(element));
}

void VertexBuffer::SetVertexCount(int vertexCount)
{
	bufferSize = vertexCount * attribute->GetVertexSize();
	this->vertexCount = vertexCount;
	byte* newBuffer = new byte[bufferSize]{0};

	if (buffer != nullptr)
	{
		memcpy(newBuffer, buffer, glm::min(bufferSize, this->vertexCount * attribute->GetVertexSize()));
		delete buffer;
	}
	buffer = newBuffer;
	this->vertexCount = vertexCount;
}
