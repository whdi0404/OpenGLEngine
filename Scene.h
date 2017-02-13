#pragma once
#include "Transform.h"
class Camera;
class Octree;
class Material;
class Scene :
	public Transform
{
//public:
//	Scene();
//	~Scene();

public:
	virtual void Initialize() {}
	virtual void Update() {}
	virtual void OnDrawGizmos() {}
/*
private:
	Camera* camera;
	Material* terrainMaterial;

	Transform* boneRoot;
	Mesh* gizmo;*/
};