#include "stdafx.h"
#include "SceneGraph.h"
#include "Time.h"
#include "Gizmo.h"
#include "kernel.cuh"

// GLFW ������ �ڵ鷯
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

int cudaTest()
{
	const int arraySize = 5;
	const int a[arraySize] = { 1, 2, 3, 4, 5 };
	const int b[arraySize] = { 10, 20, 30, 40, 50 };
	int c[arraySize] = { 0 };

	// Add vectors in parallel.
	cudaError_t cudaStatus = addWithCuda(c, a, b, arraySize);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "addWithCuda failed!");
		return 1;
	}

	printf("{1,2,3,4,5} + {10,20,30,40,50} = {%d,%d,%d,%d,%d}\n",
		c[0], c[1], c[2], c[3], c[4]);

	// cudaDeviceReset must be called before exiting in order for profiling and
	// tracing tools such as Nsight and Visual Profiler to show complete traces.
	cudaStatus = cudaDeviceReset();
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaDeviceReset failed!");
		return 1;
	}

	return 0;
}

int main() {
	cudaTest();

	// ���� �ڵ鷯 ���
	glfwSetErrorCallback(ErrorCallback);

	// GLFW �ʱ�ȭ
	if (!glfwInit())
		exit(EXIT_FAILURE);

	// ������ ����
	g_Window = glfwCreateWindow(1280, 720, "Rendering Test", NULL, NULL);
	if (!g_Window) {
		// ���� ������ ������ �����ߴٸ� ���ø����̼� ����
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	// ���ؽ�Ʈ ����
	glfwMakeContextCurrent(g_Window);

	// swap ���� ����
	glfwSwapInterval(0);

	// Ű �ڵ鷯 ���
	glfwSetKeyCallback(g_Window, KeyCallback);
	// ������ ������ ���� �ڵ鷯 ���
	glfwSetWindowSizeCallback(g_Window, WindowSizeChangeCallback);

	// GLEW �ʱ�ȭ
	glewInit();

	g_Renderer = new Renderer();
	g_Renderer->Initialize();
	g_PhysXManager = new PhysXManager();
	g_PhysXManager->PreRender();
	Gizmo::Initialize();
	SceneGraph::GetInstance().Initialize();

	auto& timeInstance = Time::GetInstance();

	//float renderTime;
	//float updateTime;
	float startTime;
	while (!glfwWindowShouldClose(g_Window))
	{
		timeInstance.Update();
		//startTime = timeInstance.GetNowTimeSinceStart();
		g_PhysXManager->PreRender();
		SceneGraph::GetInstance().Update();
		//updateTime = timeInstance.GetNowTimeSinceStart();
		//renderTime = timeInstance.GetNowTimeSinceStart();
		g_PhysXManager->PostRender();
		SceneGraph::GetInstance().Render();

		//int state = glfwGetKey(g_Window, GLFW_KEY_Y);
		//if (state == GLFW_PRESS)
		//{
		//	renderTime -= updateTime;
		//	updateTime -= startTime;
		//	std::cout << "updateTime: " << updateTime * 1000 << "ms" << "renderTime: " << renderTime * 1000 << "ms" << std::endl;
		//}
	}

	delete g_PhysXManager;
	delete g_Renderer;

	// ������ ����
	glfwDestroyWindow(g_Window);

	// GLFW ����
	glfwTerminate();

	return 0;
}