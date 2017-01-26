#pragma once
#include "Transform.h"
class Camera;
class Octree;
class Material;
class Scene :
	public Transform
{
	friend class Scene;
public:
	Scene();
	~Scene();

	void Initialize();
	void Update();

private:
	Camera* camera;
	Material* terrainMaterial;

	Transform* boneRoot;
	Mesh* gizmo;
};