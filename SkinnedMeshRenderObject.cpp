#include "stdafx.h"
#include "SkinnedMeshRenderObject.h"
#include "SkinnedMesh.h"
#include "Avatar.h"
#include "Camera.h"

SkinnedMeshRenderObject::SkinnedMeshRenderObject()
{
	isInstancing = false;
}

SkinnedMeshRenderObject::~SkinnedMeshRenderObject()
{
}

void SkinnedMeshRenderObject::Render(Camera*& camera)
{
	Material* mtrl = GetMaterial();
	
	if (g_Renderer->prevMtrl != mtrl)
	{
		g_Renderer->prevMtrl = mtrl;
		mtrl->BindShader();
		mtrl->SetCameraMatrix(camera);
	}

	mtrl->SetMatrix4x4(std::string("matWorld"), &(GetTransform()->GetWorldMatrix()), 1);
	mtrl->SetMatrix4x4(std::string("matBones"), avatar->GetRenderMatrices(), avatar->GetBoneCount());
	mtrl->BindUniformValue();

	GetMesh()->Draw();
}