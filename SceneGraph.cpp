#include "stdafx.h"
#include "SceneGraph.h"
#include "GameObject.h"
#include "Octree.h"
#include "Time.h"
#include "BoneScene.h"

SceneGraph::SceneGraph()
{
	scene = new STARTSCENE_CLASSNAME();
	//재사용 가능한 옥트리 생성
	octree = new Octree(glm::vec3(4096.0f, 4096.0f, 4096.0f), 2.0f);
}


SceneGraph::~SceneGraph()
{
}

void SceneGraph::Initialize()
{
	double t = Time::GetInstance().GetNowTimeSinceStart();
	scene->Initialize();
	t = Time::GetInstance().GetNowTimeSinceStart() - t;
	Time::GetInstance().Update();
	std::cout << t << "second";
}

void SceneGraph::Update()
{
	glfwPollEvents();

	scene->Update();
	for (auto& iter = updates.begin(); iter != updates.end(); ++iter)
		(*iter)->Update();

	for each (auto trans in Transform::movedTransform)
	{
		SceneGraph::GetInstance().octree->ReallocateObject(trans->gameObject);
		trans->moved = false;
	}

	Transform::movedTransform.clear();
}

void SceneGraph::Render()
{
	g_Renderer->Render(octree);
	glUseProgram(0);

	for each(auto gizmo in updates)
		gizmo->OnDrawGizmo();
	scene->OnDrawGizmos();
	glfwSwapBuffers(g_Window);
}

int SceneGraph::GetObjectCount()
{
	return octree->GetObjectCount();
}

void SceneGraph::FindObject(std::string& name, std::vector<GameObject*>& objectList)
{
	octree->FindObject(name, objectList);
}
