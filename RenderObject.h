#pragma once
#include "Component.h"
class RenderObject :
	public Component
{
	friend class Renderer;
	friend class SceneGraph;
protected:
	RenderObject();
	virtual ~RenderObject();

	virtual void Initialize();
	virtual void Release();

	GetMacroProtected(Math::Sphere*, CullSphere, cullSphere);
};