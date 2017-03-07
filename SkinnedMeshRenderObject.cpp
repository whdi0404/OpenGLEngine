#include "stdafx.h"
#include "SkinnedMeshRenderObject.h"
#include "SkinnedMesh.h"
#include "Avatar.h"

SkinnedMeshRenderObject::SkinnedMeshRenderObject()
{
}


SkinnedMeshRenderObject::~SkinnedMeshRenderObject()
{
}

void SkinnedMeshRenderObject::Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera)
{
	for (int i = 0; i < renderObjects.size(); ++i)
	{
		//if(camera->frustum.pointInFrustum(renderObjects[i]->GetTransform()->GetWorldPosition()))
		g_Renderer->matrixBuffer.push_back(renderObjects[i]->GetTransform()->GetWorldMatrix());
	}
	Material* mtrl = GetMaterial();
	if (g_Renderer->prevMtrl != mtrl)
	{
		g_Renderer->prevMtrl = mtrl;
		mtrl->BindShader();
		mtrl->SetCameraMatrix(camera);
	}

	SkinnedMesh* skinnedMesh = dynamic_cast<SkinnedMesh*>(mesh);
	if (skinnedMesh != nullptr)
	{
		int boneCount = skinnedMesh->GetAvatar()->GetBoneCount();
		mtrl->SetMatrix4x4(std::string("matBones"), skinnedMesh->GetAvatar()->GetRenderMatrices(), boneCount);
	}

	mtrl->BindUniformValue();
	GetMesh()->DrawInstance(g_Renderer->matrixBuffer.data(), g_Renderer->matrixBuffer.size());
}