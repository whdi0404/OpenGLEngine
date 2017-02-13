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
	//// 윈도우 이벤트 (키 스트로크 등) 폴링.
	glfwPollEvents();

	scene->Update();
	for (auto& iter = updates.begin(); iter != updates.end(); ++iter)
		(*iter)->Update();
}

void SceneGraph::Render()
{
	g_Renderer->Render(octree);

	glUseProgram(0);
	scene->OnDrawGizmos();

	// 렌더 버퍼 교체. (그린 결과를 디스플레이하는 명령)
	glfwSwapBuffers(g_Window);
}
