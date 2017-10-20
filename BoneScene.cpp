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
#include "Avatar.h"
#include "Animator.h"
#include "KeyFrameAnimation.h"
#include "TestMoving.h"
#include "RigidBody.h"
#include "TerrainSystem.h"
#include "SphereCollider.h"
#include "TerrainCollider.h"
BoneScene::BoneScene()
{
}

BoneScene::~BoneScene()
{
}

GameObject* TestObject(Mesh* mesh, Material* material, glm::vec3 offset, float scale)
{
	GameObject* obj = new GameObject();
	if (dynamic_cast<SkinnedMesh*>(mesh) != nullptr)
	{
		SkinnedMeshRenderObject* skinnedMeshRenderer = obj->AddComponent<SkinnedMeshRenderObject>();
		skinnedMeshRenderer->SetMaterial(material);
		skinnedMeshRenderer->SetMesh(mesh);
	}
	else
	{
		MeshRenderObject* meshRenderer = obj->AddComponent<MeshRenderObject>();
		meshRenderer->SetMaterial(material);
		meshRenderer->SetMesh(mesh);
	}

	obj->GetTransform()->SetLocalPosition(offset).SetLocalScale(scale, scale, scale).RotateAxisLocal(0, 0, 0);

	return obj;
}

void BoneScene::Initialize()
{
	speed = 20.0f;
	rotSpeed = 120.0f;
	camera = (new GameObject())->AddComponent<Camera>();
	camera->GetTransform()->SetLocalPosition(0, 0, 10);

	Shader* modelShader = new Shader("./Shaders/TestVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
	//Shader* skinnedShader = new Shader("./Shaders/TestSkinnedVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
	//
	material = new Material(modelShader, 1);
	//Material* skinnedMaterial = new Material(skinnedShader, 1);

	Texture2D* modelTex = new Texture2D("./Models/UnityChanTexture/body_01.tga");
	material->SetTexture(std::string("tex"), modelTex);
	//skinnedMaterial->SetTexture(std::string("tex"), modelTex);

	glm::mat4x4 modelMatrix = glm::mat4x4();
	//modelMatrix = glm::rotate(modelMatrix, -glm::radians(90.0f), glm::vec3(0, 1, 0));

	//std::vector<Object*> meshes = FBXHelper::GetResourcesFromFile("./Models/SIG.FBX", modelMatrix);
	//TestObject((Mesh*)meshes[0], material, vec3(), 1);

	//std::vector<Object*> meshes = FBXHelper::GetResourcesFromFile("./Models/unitychan.fbx", modelMatrix);
	//std::vector<Object*> animations = FBXHelper::GetResourcesFromFile("./Models/Unitychan Animation/unitychan_RUN00_F.fbx", modelMatrix);//Unitychan Animation/unitychan_RUN00_F.fbx
	gunMesh = FBXHelper::GetResourcesFromFile("./Models/Barrett.FBX", modelMatrix);
	sphereMesh = FBXHelper::GetResourcesFromFile("./Models/Sphere.FBX", modelMatrix)[0];
	//float scale = 0.1f;
	//SkinnedMesh* skinnedMesh = nullptr;
	//for (int i = 0; i < meshes.size(); ++i)
	//{
	//	Mesh* mesh = dynamic_cast<Mesh*>(meshes[i]);
	//	if (mesh == nullptr)
	//		continue;
	//
	//	if (dynamic_cast<SkinnedMesh*>(meshes[i]) != nullptr)
	//		GameObject* obj = TestObject(skinnedMesh = (SkinnedMesh*)mesh, skinnedMaterial, glm::vec3(), scale);
	//	else
	//		GameObject* obj = TestObject(mesh, material, glm::vec3(), scale);
	//}

	//KeyFrameAnimation* keyFrameAnimation = (KeyFrameAnimation*)(*std::find_if(animations.begin(), animations.end(), [](Object* obj) -> bool
	//{
	//	return dynamic_cast<KeyFrameAnimation*>(obj) != nullptr;
	//}));
	//
	//Avatar* avatar = (Avatar*)(*std::find_if(meshes.begin(), meshes.end(), [](Object* obj) -> bool
	//{
	//	return dynamic_cast<Avatar*>(obj) != nullptr;
	//}));
	//
	//Animator* animator = (new GameObject())->AddComponent<Animator>();
	//animator->SetAvatar(avatar);
	//animator->SetNowAnimation(keyFrameAnimation);

	//testModel->GetTransform()->SetLocalScale(0.01f, 0.01f, 0.01f);
	//root = nullptr;
	//if (skinnedMesh != nullptr)
	//{
	//	root = skinnedMesh->GetAvatar()->GetRoot();
	//	//root->SetLocalScale(scale, scale, scale);
	//}
	{
		GameObject* obj = new GameObject();
		TerrainSystem* terrainSystem = obj->AddComponent<TerrainSystem>();

		Shader* tessellationShader = new Shader("./Shaders/TessVetexShader.glsl", "./Shaders/TessFragmentShader.glsl",
			"./Shaders/TessCtrlShader.glsl", "./Shaders/TessEvelShader.glsl");
		float heightScale = 50;
		float tileScale = 1;

		Texture2D* heightMap = new Texture2D("./Tex/terrain-heightmap.bmp"/*cDsYZ.jpg*/);
		Material* terrainMaterial = new Material(tessellationShader, 1);
		terrainMaterial->SetTexture(std::string("heightMap"), heightMap);
		terrainSystem->SetMaterial(terrainMaterial);
		terrainSystem->CreateMesh(heightMap, /*0.03125f*/tileScale, heightScale);

		obj->AddComponent<TerrainCollider>();
		/*obj->AddComponent<TestMoving>()->SetCamera(camera);*/
		//obj->GetTransform()->SetLocalPosition(offset).SetLocalScale(scale, scale, scale).RotateAxisLocal(0, 0, 0);

		int div = 8;
		for (int x = 0; x < heightMap->GetWidth() / div; ++x)
		{
			for (int z = 0; z < heightMap->GetHeight() / div; ++z)
			{
				GameObject* obj = TestObject((Mesh*)sphereMesh, material, glm::vec3(x * div * tileScale, heightScale * 1.1f, z * div* tileScale), 1);
				obj->AddComponent<SphereCollider>()->SetCamera(camera);
			}
		}
	}
}

void BoneScene::Update()
{
	bool camChanged = false;

	float dt = Time::GetInstance().GetDeltaTime();
	float nowSpeed = speed;
	int state = glfwGetKey(g_Window, GLFW_KEY_LEFT_SHIFT);
	if (state == GLFW_PRESS)
	{
		nowSpeed = speed * 5;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_W);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->AddLocalPosition(0, 0, -nowSpeed *dt);
		camChanged = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_S);
	if (state == GLFW_PRESS)
	{
		camera->GetTransform()->AddLocalPosition(0, 0, nowSpeed *dt);
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

	state = glfwGetMouseButton(g_Window, GLFW_MOUSE_BUTTON_LEFT);
	static int oldState = GLFW_RELEASE;
	if (state == GLFW_PRESS && oldState == GLFW_RELEASE)
	{
		//for (int i = 0; i < 100; ++i)
		//{
			//GameObject* gun = new GameObject();
		//for each (Object* var in gunMesh)
		//{
		//	if (dynamic_cast<Mesh*>(var) != nullptr)
		//	{
		//		GameObject* obj = TestObject((Mesh*)var, material, camera->GetTransform()->GetWorldPosition(), 1);
		//		obj->AddComponent<TestMoving>()->SetCamera(camera);
		//		//gun->GetTransform()->AddChild(obj->GetTransform(), false);
		//	}
		//}
		GameObject* obj = TestObject((Mesh*)sphereMesh, material, camera->GetTransform()->GetWorldPosition(), 1);
		obj->AddComponent<SphereCollider>()->SetCamera(camera);
		//}
		//gun->AddComponent<TestMoving>();
		//gun->AddComponent<PhysXActor>()->CreateToBox();
		std::cout << "ObjectCount: " << SceneGraph::GetInstance().GetObjectCount() << std::endl;
	}
	oldState = state;

	state = glfwGetKey(g_Window, GLFW_KEY_T);
	/*if (state == GLFW_PRESS)
	{
		auto& TimeManager = Time::GetInstance();
		double startTime = TimeManager.GetNowTimeSinceStart();
		for(int i = 0; i < 100; ++i)
			macMove500(stressTest);
		std::cout << "CalcTransform: " << (TimeManager.GetNowTimeSinceStart() - startTime) * 1000 << "ms" << std::endl;
	}*/
}

void BoneScene::OnDrawGizmos()
{
	if (root == nullptr)
		return;
	/*std::stack<Transform*> transStack = std::stack<Transform*>();
	transStack.push(root);

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
	}*/
}