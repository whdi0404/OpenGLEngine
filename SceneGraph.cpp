#include "stdafx.h"
#include "SceneGraph.h"
#include "GameObject.h"
#include "Octree.h"
#include "Time.h"
#include "BoneScene.h"

SceneGraph::SceneGraph()
{
	scene = new STARTSCENE_CLASSNAME();
	//���� ������ ��Ʈ�� ����
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
	//// ������ �̺�Ʈ (Ű ��Ʈ��ũ ��) ����.
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

	// ���� ���� ��ü. (�׸� ����� ���÷����ϴ� ���)
	glfwSwapBuffers(g_Window);
}
