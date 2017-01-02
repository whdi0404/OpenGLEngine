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

	Get(GameObject*, GameObject, gameObject)
	Get(Transform*, Transform, transform)
	GetSet(bool, Enable, enable);
};