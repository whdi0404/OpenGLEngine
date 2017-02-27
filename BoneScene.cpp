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
#include "SkinnedMeshRenderObject.h"

BoneScene::BoneScene()
{
}

BoneScene::~BoneScene()
{
}

GameObject* TestObject(Mesh* mesh, Material* material, vec3 offset, float scale)
{
	GameObject* obj = new GameObject();
	SkinnedMeshRenderObject* meshRenderer = obj->AddComponent<SkinnedMeshRenderObject>();

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

	Texture2D* modelTex = new Texture2D("./Models/girl_1/girl1_d.tga");
	material->SetTexture(std::string("tex"), modelTex);
	skinnedMaterial->SetTexture(std::string("tex"), modelTex);

	glm::mat4x4 modelMatrix = glm::mat4x4();
	//modelMatrix = glm::rotate(modelMatrix, -glm::radians(90.0f), glm::vec3(0, 1, 0));

	//std::vector<Object*> meshes = FBXHelper::GetResourcesFromFile("./Models/SIG.FBX", modelMatrix);
	//TestObject((Mesh*)meshes[0], material, vec3(), 1);

	std::vector<Object*> meshes = FBXHelper::GetResourcesFromFile("./Models/1 (1).FBX", modelMatrix);
	SkinnedMesh* skinnedMesh = dynamic_cast<SkinnedMesh*>(meshes[0]);
	GameObject* testModel = TestObject(skinnedMesh, skinnedMaterial, vec3(), 1.0f);
	//testModel->GetTransform()->SetLocalScale(0.01f, 0.01f, 0.01f);

	root = skinnedMesh->GetRoot();
	//root->SetLocalScale(1.0f , 1.0f, 1.0f);
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
	/*Gizmo::DrawLine(camera, vec3(0, 0, 0), vec3(3, 3, 3));
*/
	std::stack<Transform*> transStack = std::stack<Transform*>();
	transStack.push(root);
	//transStack.push(trans->GetTransform());

	while (transStack.empty() == false)
	{
		auto parent = transStack.top();
		transStack.pop();

		for (int i = 0; i < parent->GetChildCount(); ++i)
		{
			auto child = parent->GetChild(i);

			Gizmo::SetColor(float((rand() % 255)) / 255.0f, float((rand() % 255)) / 255.0f, float((rand() % 255)) / 255.0f);
			Gizmo::DrawLine(camera, parent->GetWorldPosition(), child->GetWorldPosition());

			if (child->GetChildCount() > 0)
				transStack.push(child);
		}
	}
}