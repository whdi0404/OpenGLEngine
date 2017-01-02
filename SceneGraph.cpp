#include "stdafx.h"
#include "SceneGraph.h"
#include "GameObject.h"
#include "Scene.h"
#include "Octree.h"
#include "Time.h"

SceneGraph::SceneGraph()
{
	scene = new Scene();
	//재사용 가능한 옥트리 생성
	octree = new Octree(vec3(4096.0f, 4096.0f, 4096.0f), 2.0f);
}


SceneGraph::~SceneGraph()
{
}

void SceneGraph::Initialize()
{
	double t = Time::GetInstance().GetNowTimeSinceStart();
	scene->Initialize();
	t = Time::GetInstance().GetNowTimeSinceStart() - t;
	std::cout << t << "second";
}

void SceneGraph::Update()
{
	scene->Update();
	for (auto& iter = updates.begin(); iter != updates.end(); ++iter)
		(*iter)->Update();
}

void SceneGraph::Render()
{
	g_Renderer->Render(octree);
}
