#include "stdafx.h"
#include "SceneGraph.h"
#include "Time.h"
#include "Gizmo.h"

// GLFW 윈도우 핸들러
GLFWwindow* g_Window = nullptr;
Renderer* g_Renderer = nullptr;
PhysXManager* g_PhysXManager = nullptr;
FbxManager* g_FbxSdkManager = nullptr;

void ErrorCallback(int error, const char* description) {
	std::cout << description << std::endl;
}

void KeyCallback(GLFWwindow* g_Window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(g_Window, GL_TRUE);
}

void WindowSizeChangeCallback(GLFWwindow* g_Window, int width, int height) {
	glViewport(0, 0, width, height);
}


int main() {
	// 에러 핸들러 등록
	glfwSetErrorCallback(ErrorCallback);

	// GLFW 초기화
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// 윈도우 생성
	g_Window = glfwCreateWindow(1280, 720, "Rendering Test", NULL, NULL);
	if (!g_Window) {
		// 만약 윈도우 생성에 실패했다면 어플리케이션 종료
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// 컨텍스트 생성
	glfwMakeContextCurrent(g_Window);

	// swap 간격 조정
	glfwSwapInterval(0);

	// 키 핸들러 등록
	glfwSetKeyCallback(g_Window, KeyCallback);
	// 윈도우 사이즈 변경 핸들러 등록
	glfwSetWindowSizeCallback(g_Window, WindowSizeChangeCallback);

	// GLEW 초기화
	glewInit();

	g_Renderer = new Renderer();
	g_Renderer->Initialize();
	g_PhysXManager = new PhysXManager();
	g_PhysXManager->PreRender();
	Gizmo::Initialize();
	SceneGraph::GetInstance().Initialize();

	while (!glfwWindowShouldClose(g_Window))
	{
		Time::GetInstance().Update();
		g_PhysXManager->PreRender();
		SceneGraph::GetInstance().Update();
		SceneGraph::GetInstance().Render();
		g_PhysXManager->PostRender();
	}

	delete g_PhysXManager;
	delete g_Renderer;

	// 윈도우 제거
	glfwDestroyWindow(g_Window);

	// GLFW 종료
	glfwTerminate();

	return 0;
}