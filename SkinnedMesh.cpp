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
		boneMatrices[i] = boneTransforms[i]->GetWorldMatrix() * deformerMatrices[i];
}

void SkinnedMesh::AddBone(Transform * boneTransform, mat4x4 deformerMatrix)
{
	boneTransforms.push_back(boneTransform);
	deformerMatrices.push_back(deformerMatrix);
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
