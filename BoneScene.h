#pragma once
#include "Scene.h"
class Camera;
class BoneScene :
	public Scene
{
public:
	BoneScene();
	~BoneScene();

	void Initialize();
	void Update();
	void OnDrawGizmos();

	Camera* camera;

	float speed;
	float rotSpeed;

	Transform* root;
	Material* material;
	std::vector<Object*> gunMesh;
	Object* sphereMesh;

	GameObject* stressTest;
};