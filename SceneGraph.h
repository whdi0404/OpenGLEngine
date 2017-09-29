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

	void Update();
	
	void Render();

	int GetObjectCount();

	void FindObject(std::string& name, std::vector<GameObject*>& objectList);

private:
	std::vector<Component*> updates;
	Scene* scene;
	Octree* octree;
};