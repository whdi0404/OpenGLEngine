#include "stdafx.h"
#include "Mesh.h"
#include "AttributeBase.h"
#include "Texture.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "FBXHelper.h"
#include "VertexBuffer.h"

unsigned short Mesh::totalResourceID = 0;

Mesh::Mesh() : vertexBuffer(nullptr), drawMode(GL_TRIANGLES)
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
	glVertexAttribPointer(matrixStartAttributePointer + 0, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * 0));
	glVertexAttribPointer(matrixStartAttributePointer + 1, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * 1));
	glVertexAttribPointer(matrixStartAttributePointer + 2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * 2));
	glVertexAttribPointer(matrixStartAttributePointer + 3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * 3));
	glVertexAttribDivisor(matrixStartAttributePointer + 0, 1);
	glVertexAttribDivisor(matrixStartAttributePointer + 1, 1);
	glVertexAttribDivisor(matrixStartAttributePointer + 2, 1);
	glVertexAttribDivisor(matrixStartAttributePointer + 3, 1);

	glBindVertexArray(0);
}

void Mesh::DrawInstance(mat4x4* pMat, int count)
{
	//glm::mat4 Proj = glm::perspective(glm::radians(45.0f), (float)1024.0f / (float)768.0f, 0.1f, 5000.0f);
	//glm::mat4 View = glm::inverse(camTrans.GetWorldMatrix());

	////BindShaderProgram
	//glUseProgram(shaderProgamID);
	//

	////BindView,Proj Matrices
	//GLuint matProj = glGetUniformLocation(shaderProgramID, "matProj");
	//glUniformMatrix4fv(matProj, 1, GL_FALSE, &Proj[0][0]);

	//GLuint matView = glGetUniformLocation(shaderProgramID, "matView");
	//glUniformMatrix4fv(matView, 1, GL_FALSE, &View[0][0]);

	glBindVertexArray(vertexArrayID);

	//BindMatrix
	glBindBuffer(GL_ARRAY_BUFFER, matrixBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * count, pMat, GL_STATIC_DRAW);

	//Texture Bind
	/*GLuint ShaderTexID = glGetUniformLocation(shaderProgramID, "myTextureSampler");
	glUniform1i(ShaderTexID, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
	glBindSampler(ShaderTexID, texture->GetSamplerID());*/

	//Draw
	glDrawElementsInstanced(
		drawMode,      // mode
		indices.size(),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0,           // element array buffer offset
		count
	);

	glBindVertexArray(0);
}