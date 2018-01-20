#include "stdafx.h"
#include "SkinnedMeshRenderObject.h"
#include "SkinnedMesh.h"
#include "Avatar.h"
#include "Camera.h"

glm::mat4x4* SkinnedMeshRenderObject::skinnedMatrixBuffer = nullptr;
int SkinnedMeshRenderObject::bufferSize = 0;

SkinnedMeshRenderObject::SkinnedMeshRenderObject()
{
}

SkinnedMeshRenderObject::~SkinnedMeshRenderObject()
{
}

void SkinnedMeshRenderObject::Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera)
{

	int boneCount = avatar->GetBoneCount();

	int needBufferSize = boneCount * renderObjects.size() * sizeof(glm::mat4x4);
	int oneBonesSize = boneCount * sizeof(glm::mat4x4);

	if (skinnedMatrixBuffer == nullptr || bufferSize < needBufferSize)
	{
		delete[] skinnedMatrixBuffer;
		bufferSize = needBufferSize;
		skinnedMatrixBuffer = new glm::mat4x4[bufferSize]{ glm::mat4x4() };
	}

	for (int i = 0; i < renderObjects.size(); ++i)
	{
		g_Renderer->matrixBuffer.emplace_back(renderObjects[i]->GetTransform()->GetWorldMatrix());
		
		SkinnedMeshRenderObject* skinnedMeshRenderObj = (SkinnedMeshRenderObject*)renderObjects[i];
		memcpy_s(&skinnedMatrixBuffer[boneCount * i], bufferSize - (oneBonesSize * i), skinnedMeshRenderObj->GetAvatar()->GetRenderMatrices(), oneBonesSize);
	}

	Material* mtrl = GetMaterial();
	if (g_Renderer->prevMtrl != mtrl)
	{
		g_Renderer->prevMtrl = mtrl;
		mtrl->BindShader();
		mtrl->SetCameraMatrix(camera);
	}

	mtrl->SetInt(std::string("boneCount"), &boneCount, 1);
	mtrl->SetMatrix4x4(std::string("matBones"), avatar->GetRenderMatrices(), avatar->GetBoneCount());
	mtrl->SetSSBOData(1, skinnedMatrixBuffer, needBufferSize);

	mesh->DrawInstance(g_Renderer->matrixBuffer.data(), g_Renderer->matrixBuffer.size());
}