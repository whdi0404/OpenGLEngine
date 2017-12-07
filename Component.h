#pragma once
#include "Object.h"
class Collider;
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
	virtual void OnDrawGizmo() {}
	virtual void Release() {}

	virtual void OnTriggerEnter(Collider* collider) {}
	virtual void OnTriggerExit(Collider* collider) {}

	template<class TComponent>
	TComponent* GetComponent()
	{
		if(gameObject != nullptr)
			return gameObject->GetComponent<TComponent>();
		return nullptr;
	}

	GetMacro(GameObject*, GameObject, gameObject)
	GetMacro(Transform*, Transform, transform)
	GetSetMacro(bool, Enable, enable);
};