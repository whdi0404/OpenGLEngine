#include "stdafx.h"
#include "Mesh.h"
#include "Texture.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "FBXHelper.h"
#include "VertexBuffer.h"

unsigned short Mesh::totalResourceID = 0;

Mesh::Mesh() : vertexBuffer(nullptr), drawMode(GL_TRIANGLES), boundingSphere({glm::vec3(0, 0, 0), 0})
{
	resourceID = totalResourceID++;
}
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vertexArrayID);
}
void Mesh::SetMeshData(VertexBuffer* vertexBuffer, std::vector<int>& indices)
{
	if (this->vertexBuffer != nullptr)
		delete this->vertexBuffer;
	this->indices.clear();
	this->indices = indices;
	glBindVertexArray(0);

	this->vertexBuffer = vertexBuffer;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	GLuint vertices;
	glGenBuffers(1, &vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer->GetBufferSize(), vertexBuffer->GetBufferPointer(), GL_STATIC_DRAW);
	vertexBuffer->GetAttribute()->EnableGLVertexAttributePointer();

	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(*this->indices.data()) * this->indices.size(), this->indices.data(), GL_STATIC_DRAW);

	int matrixStartAttributePointer = vertexBuffer->GetAttribute()->GetElementCount();

	glGenBuffers(1, &matrixBufferID);
	glBindBuffer(GL_ARRAY_BUFFER, matrixBufferID);
	glEnableVertexAttribArray(matrixStartAttributePointer + 0);
	glEnableVertexAttribArray(matrixStartAttributePointer + 1);
	glEnableVertexAttribArray(matrixStartAttributePointer + 2);
	glEnableVertexAttribArray(matrixStartAttributePointer + 3);
	glVertexAttribPointer(matrixStartAttributePointer + 0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4x4), (void*)(sizeof(glm::vec4) * 0));
	glVertexAttribPointer(matrixStartAttributePointer + 1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4x4), (void*)(sizeof(glm::vec4) * 1));
	glVertexAttribPointer(matrixStartAttributePointer + 2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4x4), (void*)(sizeof(glm::vec4) * 2));
	glVertexAttribPointer(matrixStartAttributePointer + 3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4x4), (void*)(sizeof(glm::vec4) * 3));
	glVertexAttribDivisor(matrixStartAttributePointer + 0, 1);
	glVertexAttribDivisor(matrixStartAttributePointer + 1, 1);
	glVertexAttribDivisor(matrixStartAttributePointer + 2, 1);
	glVertexAttribDivisor(matrixStartAttributePointer + 3, 1);

	glBindVertexArray(0);

	boundingSphere = Math::Sphere::CreateBoundingSphere(vertexBuffer);
}

void Mesh::DrawInstance(glm::mat4x4* pMat, int count)
{
	glBindVertexArray(vertexArrayID);

	//BindMatrix
	glBindBuffer(GL_ARRAY_BUFFER, matrixBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * count, pMat, GL_STATIC_DRAW);

	//Draw
	glDrawElementsInstanced(
		drawMode,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0,      // element array buffer offset
		count
	);

	glBindVertexArray(0);
}