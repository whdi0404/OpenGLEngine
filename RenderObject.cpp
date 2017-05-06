#include "stdafx.h"
#include "RenderObject.h"


RenderObject::RenderObject() : cullSphere(nullptr)
{
}


RenderObject::~RenderObject()
{
}

void RenderObject::Initialize()
{
	g_Renderer->GetCollector()->RegisterRenderObject(this);
}

void RenderObject::Release()
{
}