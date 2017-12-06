#pragma once
#include "MeshVertexAttribute.h"
#include <vector>

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

	template<typename T>
	std::vector<T> GetVectorList(Element element)
	{
	
	}

	template<> 
	std::vector<glm::vec2> VertexBuffer::GetVectorList(Element element)
	{
		std::vector<glm::vec2> retVal;
		for (int i = 0; i < vertexCount; ++i)
		{
			glm::vec2 newVec;
			glm::vec4 v = GetVector(element, i);
			newVec.x = v.x;
			newVec.y = v.y;
			retVal.push_back(newVec);
		}

		return retVal;
	}

	template<> 
	std::vector<glm::vec3> VertexBuffer::GetVectorList(Element element)
	{
		std::vector<glm::vec3> retVal;
		for (int i = 0; i < vertexCount; ++i)
		{
			glm::vec3 newVec;
			glm::vec4 v = GetVector(element, i);
			newVec.x = v.x;
			newVec.y = v.y;
			newVec.z = v.z;
			retVal.push_back(newVec);
		}

		return retVal;
	}

	void SetVector(Element element, int index, glm::vec4 vector);
	void SetVertexCount(int vertexCount);
};
