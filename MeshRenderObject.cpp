#include "stdafx.h"
#include "MeshRenderObject.h"
#include "SkinnedMesh.h"
#include "Camera.h"

MeshRenderObject::MeshRenderObject()
{
}

MeshRenderObject::~MeshRenderObject()
{
}

void MeshRenderObject::Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera)
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

	mtrl->BindUniformValue();
	mesh->DrawInstance(g_Renderer->matrixBuffer.data(), g_Renderer->matrixBuffer.size());
}