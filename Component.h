#pragma once
#include "Object.h"
class Component :
	public Object
{
	friend class GameObject;
	friend class SceneGraph;
protected:
	Component();
	virtual ~Component();

	virtual void Initialize() {}
	virtual void Update() {}
	virtual void Release() {}

	GetMacro(GameObject*, GameObject, gameObject)
	GetMacro(Transform*, Transform, transform)
	GetSetMacro(bool, Enable, enable);
};