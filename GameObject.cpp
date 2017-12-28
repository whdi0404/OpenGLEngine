#include "stdafx.h"
#include "GameObject.h"
#include "SceneGraph.h"
#include "Scene.h"
#include "Octree.h"

GameObject::GameObject()
{
	Init();
}

GameObject::GameObject(std::string name)
{
	Init();
	this->SetName(name);
}

GameObject::~GameObject()
{
	if (transform == nullptr)
		return;
}

void GameObject::Init()
{
	renderObject = nullptr; 
	octreeLocationCode = 0;
	cullSphere.center = glm::vec3(0, 0, 0);
	cullSphere.radius = 0;
	transform = new Transform();
	transform->gameObject = this;

	//SceneGraph::GetInstance().scene->AddChild(transform, false);
	SceneGraph::GetInstance().octree->AddObject(this);
}
