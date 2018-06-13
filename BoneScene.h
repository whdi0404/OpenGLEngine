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

	Camera* camera;

	float speed;
	float rotSpeed;

	Transform* avatarRoot;
	Material* material;
	Material* terrainMaterial;
	std::vector<Object*> gunMesh;
	Object* sphereMesh;
	Avatar* avatar;

	GameObject* stressTest;

	KeyFrameAnimation* keyFrameAnimation;
	std::vector<Object*> meshes;
	Material* skinnedMaterial;

	Texture2D* heightMap;
	Texture2D* heightSeed;
	Texture2D* resizeTexture;
	float xyWeight;
	float octaves;
	float amplitude; 
	float frequency; 
	float h;
	bool isFractal;
};