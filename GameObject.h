#pragma once
#include "Object.h"
#include "Component.h"
#include "RenderObject.h"
#include "SceneGraph.h"

class GameObject :
	public Object
{
	friend class SceneGraph;
	friend class GameObject;
	friend class Octree;
	friend class Transform;
public:
	GameObject();
	~GameObject();

	template<typename TComponent>
	TComponent* AddComponent();

	template<typename TComponent>
	TComponent* GetComponent();
private:
	//void Update();

private:
	std::vector<Component*> v_Components;
	RenderObject* renderObject;
	//Todo : Collider �߰�, Collider�� renderObject������ ���� ū halfSize
	//�� ������ ��Ʈ���� �߰�.

	GetMacro(Transform*, Transform, transform);
	GetSetMacro(bool, Active, active);
	GetSetMacro(bool, Static, isStatic);
	GetSetMacro(Math::Sphere, CullSphere, cullSphere);

	GetSetMacro(uint32_t, OctreeLocCode, octreeLocationCode);
};

template<typename TComponent>
inline TComponent* GameObject::AddComponent()
{
	TComponent* newComponent = new TComponent();

	if (dynamic_cast<Component*>(newComponent) == nullptr)
		return nullptr;

	Component* newComp = (Component*)newComponent;
	newComp->gameObject = this;
	newComp->transform = this->transform;

	if (dynamic_cast<RenderObject*>(newComponent) == nullptr)
	{
		v_Components.emplace_back((Component*)newComponent);
	}
	else
	{
		renderObject = (RenderObject*)newComponent;
	}

	newComp->Initialize();

	/*IsOverridden<Component, Component::Update, TComponent, Component::Update>();

	void (Component::*fpA)();
	void (TComponent::*fpB)();
*/
	SceneGraph::GetInstance().updates.emplace_back(newComp);

	return newComponent;
}

template<typename TComponent>
inline TComponent * GameObject::GetComponent()
{
	for (int i = 0; i < v_Components.size(); ++i)
	{
		if (dynamic_cast<TComponent*>(v_Components[i]) != nullptr)
			return (TComponent*)v_Components[i];
	}
}