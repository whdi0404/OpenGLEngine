#include "stdafx.h"
#include "SkinnedMesh.h"
#include "VertexBuffer.h"

SkinnedMesh::SkinnedMesh()
{
}


SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::SetAvatar(Avatar * avatar)
{
	this->avatar = avatar;
}