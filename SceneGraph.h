#pragma once
#include "SingletonBase.h"

class RenderObject;
class Scene;
class Octree;
class SceneGraph : public Singleton<SceneGraph>
{
	friend class Transform;
	friend class GameObject;
public:
	SceneGraph();
	~SceneGraph();

	void Initialize();

	//Update,Coroutines
	void Update();
	
	void Render();

private:
	std::unordered_set<Component*> updates;
	Scene* scene;
	Octree* octree;
};