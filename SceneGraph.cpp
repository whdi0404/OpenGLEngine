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

	float reallocToOctreeTime, updateTime;
	float startTime = Time::GetInstance().GetNowTimeSinceStart();
	scene->Update();
	int size = updates.size();
	for (int i = 0; i < size; ++i)
		updates[i]->Update();
	updateTime = Time::GetInstance().GetNowTimeSinceStart();

	for each (auto& trans in Transform::movedTransform)
	{
		trans->RecalcuateBoundingSphere(); 
		SceneGraph::GetInstance().octree->ReallocateObject(trans->gameObject);
		trans->moved = false;
	}
	Transform::movedTransform.clear();
	reallocToOctreeTime = Time::GetInstance().GetNowTimeSinceStart();
	int state = glfwGetKey(g_Window, GLFW_KEY_Y);
	if (state == GLFW_PRESS)
	{
		reallocToOctreeTime -= updateTime;
		updateTime -= startTime;

		std::cout << "updateTime: " << updateTime * 1000 << "ms reallocTime: " << reallocToOctreeTime * 1000 << "ms" << std::endl;
	}
}

void SceneGraph::Render()
{
	g_Renderer->Render(octree);
	glUseProgram(0);

	//int size = updates.size();
	//for (int i = 0; i < size; ++i)
	//	updates[i]->OnDrawGizmo();
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