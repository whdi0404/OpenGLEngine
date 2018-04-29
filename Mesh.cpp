#include "stdafx.h"
#include "Mesh.h"
#include "Texture.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "FBXHelper.h"
#include "VertexBuffer.h"

unsigned short Mesh::totalResourceID = 0;

Mesh::Mesh() : vertexBuffer(nullptr), drawMode(GL_TRIANGLES), boundingSphere({glm::vec3(0, 0, 0), 0}), textures(nullptr)
{
	resourceID = totalResourceID++;
}
Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &vertexArrayID);

	delete textures;
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

	CalculateNormal();
	CalculateTangent();

	CalculateBound();
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

void Mesh::Draw()
{
	glBindVertexArray(vertexArrayID);

	//BindMatrix
	glBindBuffer(GL_ARRAY_BUFFER, matrixBufferID);

	//Draw
	glDrawElements(
		drawMode,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0
	);

	glBindVertexArray(0);
}

void Mesh::AddTexture(Texture2D * texture)
{
	if (textures == nullptr)
	{
		textures = new std::vector<Texture2D*>();
	}

	textures->push_back(texture);
}

void Mesh::CalculateNormal()
{
	//Quad에 대한 작업 필요함.
	for (int i = 0; i < indices.size(); i+=3)
	{
		int idx1 = indices[i];
		int idx2 = indices[i + 1];
		int idx3 = indices[i + 2];
		glm::vec3 v1 = (glm::vec3)vertexBuffer->GetVector(Element::Position, idx1);
		glm::vec3 v2 = (glm::vec3)vertexBuffer->GetVector(Element::Position, idx2);
		glm::vec3 v3 = (glm::vec3)vertexBuffer->GetVector(Element::Position, idx3);

		glm::vec3 norm = glm::normalize(glm::cross(v2 - v1, v3 - v1));

		glm::vec3 n1 = (glm::vec3)vertexBuffer->GetVector(Element::Normal, idx1);
		glm::vec3 n2 = (glm::vec3)vertexBuffer->GetVector(Element::Normal, idx2);
		glm::vec3 n3 = (glm::vec3)vertexBuffer->GetVector(Element::Normal, idx3);

		n1 = glm::normalize(n1 + norm);
		n2 = glm::normalize(n2 + norm);
		n3 = glm::normalize(n3 + norm);

		vertexBuffer->SetVector(Element::Normal, idx1, glm::vec4(n1.x, n1.y, n1.z, 0));
		vertexBuffer->SetVector(Element::Normal, idx2, glm::vec4(n2.x, n2.y, n2.z, 0));
		vertexBuffer->SetVector(Element::Normal, idx3, glm::vec4(n3.x, n3.y, n3.z, 0));
	}
}

void Mesh::CalculateTangent()
{
	glm::vec3 *tan1 = new glm::vec3[vertexBuffer->GetVertexCount() * 2];
	glm::vec3 *tan2 = tan1 + vertexBuffer->GetVertexCount();
	ZeroMemory(tan1, vertexBuffer->GetVertexCount() * sizeof(glm::vec3) * 2);

	for (long i = 0; i < indices.size(); i+=3)
	{
		int idx1 = indices[i];
		int idx2 = indices[i + 1];
		int idx3 = indices[i + 2];

		const glm::vec3 v1 = (glm::vec3)vertexBuffer->GetVector(Element::Position, idx1);
		const glm::vec3 v2 = (glm::vec3)vertexBuffer->GetVector(Element::Position, idx2);
		const glm::vec3 v3 = (glm::vec3)vertexBuffer->GetVector(Element::Position, idx3);

		const glm::vec2 w1 = (glm::vec2)vertexBuffer->GetVector(Element::Texcoord0, idx1);
		const glm::vec2 w2 = (glm::vec2)vertexBuffer->GetVector(Element::Texcoord1, idx2);
		const glm::vec2 w3 = (glm::vec2)vertexBuffer->GetVector(Element::Texcoord2, idx3);

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		glm::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
			(t2 * z1 - t1 * z2) * r);
		glm::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
			(s1 * z2 - s2 * z1) * r);

		tan1[idx1] += sdir;
		tan1[idx2] += sdir;
		tan1[idx3] += sdir;

		tan2[idx1] += tdir;
		tan2[idx2] += tdir;
		tan2[idx3] += tdir;
	}

	for (long i = 0; i < vertexBuffer->GetVertexCount(); i++)
	{
		glm::vec3 n = (glm::vec3)vertexBuffer->GetVector(Element::Normal, i);
		glm::vec3 t = (glm::vec3)tan1[i];

		// Gram-Schmidt orthogonalize
		glm::vec3 tanNorm = glm::normalize(t - n * glm::dot(n, t));
		glm::vec4 tangent = glm::vec4(tanNorm.x, tanNorm.y, tanNorm.z, 0);

		//// Calculate handedness
		//tangent.w = (glm::dot(glm::cross(n, t), tan2[i]) < 0.0F) ? -1.0F : 1.0F;

		vertexBuffer->SetVector(Element::Tangent, i, glm::vec4(tangent.x, tangent.y, tangent.z, 0));
	}

	delete[] tan1;
}

void Mesh::CalculateBound()
{
	boundingSphere = Math::Sphere::CreateBoundingSphere(vertexBuffer);
}
