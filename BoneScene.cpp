#include "stdafx.h"
#include "BoneScene.h"
#include "GameObject.h"
#include "MeshRenderObject.h"
#include "Time.h"
#include "Camera.h"
#include "Shader.h"
#include "Texture.h"
#include "SkinnedMesh.h"
#include "Mesh.h"
#include "Gizmo.h"

BoneScene::BoneScene()
{
}


BoneScene::~BoneScene()
{
}

GameObject* TestObject(Mesh* mesh, Material* material, vec3 offset, float scale)
{
	GameObject* obj = new GameObject();
	MeshRenderObject* meshRenderer = obj->AddComponent<MeshRenderObject>();

	meshRenderer->SetMaterial(material);
	meshRenderer->SetMesh(mesh);
	obj->GetTransform()->SetLocalPosition(offset).SetLocalScale(scale, scale, scale).RotateAxisLocal(0, 0, 0);

	return obj;
}

void BoneScene::Initialize()
{
	speed = 4.0f;
	rotSpeed = 120.0f;
	camera = (new GameObject())->AddComponent<Camera>();
	camera->GetTransform()->SetLocalPosition(0, 0, 10);

	Shader* shader = new Shader("./Shaders/TestVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
	Shader* skinnedShader = new Shader("./Shaders/TestSkinnedVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
	Material* material = new Material(shader, 1);
	Material* skinnedMaterial = new Material(skinnedShader, 1);
	
	Texture2D* modelTex = new Texture2D("./Tex/Tex_0049_1.jpg");
	material->SetTexture(std::string("tex"), modelTex);
	skinnedMaterial->SetTexture(std::string("tex"), modelTex);
	std::vector<Object*> meshes = FBXHelper::GetResourcesFromFile("./Models/SIG.FBX");
	TestObject((Mesh*)meshes[0], material, vec3(), 1);
	
	std::vector<Object*> bones = FBXHelper::GetResourcesFromFile("./Models/1 (1).fbx"); 
	SkinnedMesh* skinnedMesh = dynamic_cast<SkinnedMesh*>(bones[0]);
	TestObject((Mesh*)bones[0], skinnedMaterial, vec3(), 1);
}

void BoneScene::Update()
{
	bool camChanged = false;
	
	float dt = Time::GetInstance().GetDeltaTime();
	int state = glfwGetKey(g_Window, GLFW_KEY_W);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->AddLocalPosition(0, 0, -speed *dt);
		camChanged = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_S);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->AddLocalPosition(0, 0, speed *dt);
		camChanged = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_RIGHT);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->RotateAxisLocal(0, 0, -rotSpeed *dt);
		camChanged = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_LEFT);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->RotateAxisLocal(0, 0, rotSpeed *dt);
		camChanged = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_A);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->RotateAxisLocal(0, rotSpeed *dt, 0);
		camChanged = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_D);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->RotateAxisLocal(0, -rotSpeed *dt, 0);
		camChanged = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_DOWN);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->RotateAxisLocal(rotSpeed *dt, 0, 0);
		camChanged = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_UP);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->RotateAxisLocal(-rotSpeed * dt, 0, 0);
		camChanged = true;
	}

	//if (camChanged)
	//{
	//	//camera->RefreshViewMatrix();
	//	glm::vec3 eyePos = camera->GetTransform()->GetWorldPosition();
	//	//eyePos.y = 0;

	//	//terrainMaterial->SetVec3(std::string("eyePosition"), eyePos);
	//}
}

void BoneScene::OnDrawGizmos()
{
	Gizmo::DrawLine(camera, vec3(0, 0, 0), vec3(3, 3, 3));
}