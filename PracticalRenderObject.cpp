#include "stdafx.h"
#include "PracticalRenderObject.h"

PracticalRenderObject::PracticalRenderObject() : mesh(nullptr), material(nullptr), hashCode(-1), rendererVector(nullptr)
{
}


PracticalRenderObject::~PracticalRenderObject()
{
}

void PracticalRenderObject::SetMaterial(Material * material)
{
	this->material = material;
	RefreshHashCode();
}

void PracticalRenderObject::SetMesh(Mesh * mesh)
{
	this->mesh = mesh;
	if (mesh != nullptr)
		cullSphere = &mesh->GetBoundingSphere();
	else
		cullSphere = nullptr;
	RefreshHashCode();
}

void PracticalRenderObject::AddToVector()
{
	rendererVector->push_back(this);
}

void PracticalRenderObject::RefreshHashCode()
{
	rendererVector = nullptr;
	hashCode = -1;
	if (this->material != nullptr && this->mesh != nullptr)
	{
		hashCode = ((uint32_t)mesh->GetResourceID()) | material->GetResourceID() << 16;
		
		rendererVector = g_Renderer->GetCollector()->RegisterRenderObject(this);
	}
}
