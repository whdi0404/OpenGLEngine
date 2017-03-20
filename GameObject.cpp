#include "stdafx.h"
#include "GameObject.h"
#include "SceneGraph.h"
#include "Scene.h"
#include "Octree.h"

GameObject::GameObject() : renderObject(nullptr), octreeLocationCode(0)
{
	cullSphere.center = glm::vec3(0,0,0);
	cullSphere.radius = 0;
	transform = new Transform();
	transform->gameObject = this;

	SceneGraph::GetInstance().scene->AddChild(transform, false);
	RefreshOctreeNode();
}

GameObject::~GameObject()
{
}

void GameObject::RefreshOctreeNode()
{
	SceneGraph::GetInstance().octree->DeleteObject(this);
	SceneGraph::GetInstance().octree->AddObject(this);
}