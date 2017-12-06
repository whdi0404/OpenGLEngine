#pragma once
#include "Scene.h"
class Camera;
class BoneScene :
	public Scene
{
public:
	BoneScene();
	~BoneScene();


private:
	void Initialize();
	void InitResource();
public:
	void Update();
	void OnDrawGizmos();

	Camera* camera;

	float speed;
	float rotSpeed;

	Transform* root;
	Material* material;
	Material* terrainMaterial;
	std::vector<Object*> gunMesh;
	Object* sphereMesh;

	GameObject* stressTest;
};