#include "stdafx.h"
#include "SkinnedMesh.h"
#include "VertexBuffer.h"

SkinnedMesh::SkinnedMesh() : root(nullptr)
{
}


SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::SetAvatar(Avatar * avatar)
{
	this->avatar = avatar;
}

void SkinnedMesh::SetRoot(Transform* root)
{
	this->root = root;
	
	boneMatrices = new mat4x4[boneTransforms.size()];
	for(int i=0;i < boneTransforms.size();++i)
		boneMatrices[i] = boneTransforms[i]->GetWorldMatrix();
}

void SkinnedMesh::AddBone(Transform * boneTransform)
{
	boneTransforms.push_back(boneTransform);
	std::string boneName = boneTransform->GetName();
	bones.insert(KEYVALUE(std::string, Transform*)(boneName, boneTransform));
}

Transform * SkinnedMesh::GetBone(std::string boneName)
{
	auto pair = bones.find(boneName);
	if (pair != bones.end())
		return pair->second;

	return nullptr;
}

void SkinnedMesh::DrawInstance(mat4x4 * pMat, int count)
{
	glBindVertexArray(vertexArrayID);

	//BindMatrix
	glBindBuffer(GL_ARRAY_BUFFER, matrixBufferID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * count, pMat, GL_STATIC_DRAW);

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
