#include "stdafx.h"
#include "AttributeBase.h"

void AttributeBase::Begin(GLuint vertexBuffer)
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	
	SetAttribute();
}

void AttributeBase::End()
{
	glDisableVertexAttribArray(0);
}

void VertexAttrib::SetAttribute()
{
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
}