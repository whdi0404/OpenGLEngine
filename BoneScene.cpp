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
#include "RigidBody.h"
#include "TerrainSystem.h"
#include "TerrainCollider.h"
#include "ResourceManager.h"
#include "PhysXUtil.h"
#include "PhysXJoint.h"
#include "Ragdoll.h"

BoneScene::BoneScene() : avatarRoot(nullptr)
{
}

BoneScene::~BoneScene()
{
}

GameObject* TestObject(std::string name, Mesh* mesh, Avatar* avatar, Material* material, glm::vec3 offset, float scale)
{
	GameObject* obj = new GameObject(name);
	if (dynamic_cast<SkinnedMesh*>(mesh) != nullptr)
	{
		SkinnedMeshRenderObject* skinnedMeshRenderer = obj->AddComponent<SkinnedMeshRenderObject>();
		skinnedMeshRenderer->SetMaterial(material);
		skinnedMeshRenderer->SetMesh(mesh);
		skinnedMeshRenderer->SetAvatar(avatar);
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
	InitResource();

	speed = 200.0f;
	rotSpeed = 120.0f;
	camera = (new GameObject())->AddComponent<Camera>();

	Shader* modelShader = new Shader("./Shaders/TestVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
	Shader* skinnedShader = new Shader("./Shaders/TestSkinnedVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
	//
	material = new Material(modelShader, 1);
	skinnedMaterial = new Material(skinnedShader, 1);

	Texture2D* modelTex = new Texture2D("./Models/UnityChanTexture/body_01.tga");
	material->SetTexture(std::string("tex"), modelTex);
	skinnedMaterial->SetTexture(std::string("tex"), modelTex);

	glm::mat4x4 modelMatrix = glm::mat4x4();
	//modelMatrix = glm::rotate(modelMatrix, -glm::radians(90.0f), glm::vec3(0, 1, 0));

	//std::vector<Object*> meshes = FBXHelper::GetResourcesFromFile("./Models/SIG.FBX", modelMatrix);
	//TestObject((Mesh*)meshes[0], material, vec3(), 1);

	meshes = FBXHelper::GetResourcesFromFile("./Models/unitychan.fbx", modelMatrix);
	avatar = (Avatar*)*std::find_if(meshes.begin(), meshes.end(), [](Object* obj) {return dynamic_cast<Avatar*>(obj) != nullptr; });
	//std::vector<Object*> animations = FBXHelper::GetResourcesFromFile("./Models/Unitychan Animation/unitychan_RUN00_F.fbx", modelMatrix);//Unitychan Animation/unitychan_RUN00_F.fbx
	gunMesh = FBXHelper::GetResourcesFromFile("./Models/Barrett.FBX", modelMatrix);
	sphereMesh = FBXHelper::GetResourcesFromFile("./Models/Sphere.FBX", modelMatrix)[0];

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

	
	{
		GameObject* obj = new GameObject();
		TerrainSystem* terrainSystem = obj->AddComponent<TerrainSystem>();

		Shader* tessellationShader = new Shader("./Shaders/TessVetexShader.glsl", "./Shaders/TessFragmentShader.glsl",
			"./Shaders/TessCtrlShader.glsl", "./Shaders/TessEvelShader.glsl");
		float heightScale = 100;
		float tileScale = 10;

		Texture2D* heightMap = new Texture2D("./Tex/terrain-heightmap.bmp"/*cDsYZ.jpg*/);
		terrainMaterial = new Material(tessellationShader, 1);
		terrainMaterial->SetTexture(std::string("heightMap"), heightMap);
		terrainSystem->SetMaterial(terrainMaterial);
		terrainSystem->CreateMesh(heightMap, /*0.03125f*/tileScale, heightScale, 8);

		obj->AddComponent<TerrainCollider>();

		//GameObject* obj2 = new GameObject();
		//TerrainSystem* terrainSystem2 = obj2->AddComponent<TerrainSystem>();
		//
		//terrainMaterial = new Material(tessellationShader, 1);
		//terrainMaterial->SetTexture(std::string("heightMap"), heightMap);
		//terrainSystem2->SetMaterial(terrainMaterial);
		//terrainSystem2->CreateMesh(heightMap, /*0.03125f*/tileScale, heightScale, 2);
		//obj2->AddComponent<TerrainCollider>();
		/*obj->AddComponent<TestMoving>()->SetCamera(camera);*/
		//obj->GetTransform()->SetLocalPosition(offset).SetLocalScale(scale, scale, scale).RotateAxisLocal(0, 0, 0);

		camera->GetTransform()->SetLocalPosition(tileScale* heightMap->GetWidth() * 0.5f, heightScale * 1.5f, tileScale * heightMap->GetHeight());
		camera->GetTransform()->RotateAxisLocal(-25, 0, 0);

		//int div = 7;
		//for (int x = 5; x < heightMap->GetWidth() / div; ++x)
		//{
		//	for (int z = 5; z < heightMap->GetHeight() / div; ++z)
		//	{
		//		GameObject* obj = TestObject((Mesh*)sphereMesh, material, glm::vec3(x * div * tileScale * 0.7f, heightScale * 1.1f, z * div* tileScale * 0.7f), 1);
		//		obj->AddComponent<SphereCollider>()->SetCamera(camera);
		//		//obj->AddComponent<TestMoving>()->SetCamera(camera);
		//	}
		//}
	}
}

void BoneScene::InitResource()
{
	PxMaterial* default_Material = g_PhysXManager->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
	ResourceManager::GetInstance().AddResource("default_Material", default_Material);
	
	Mesh* box_Mesh = (Mesh*)FBXHelper::GetResourcesFromFile("./Models/Box.fbx")[0];
	ResourceManager::GetInstance().AddPxConvexMeshGeometryResource("box_Mesh", box_Mesh, PxVec3(0.01f, 0.01f, 0.01f));

	ResourceManager::GetInstance().AddPxSphereGeometryResource("default_Sphere", 2.5f);
	ResourceManager::GetInstance().AddPxCapsuleGeometryResource("default_Capsule", 0.5f, 0.5f);
	ResourceManager::GetInstance().AddPxBoxGeometryResource("default_Box", glm::vec3(0.5f, 0.5f, 0.5f));
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
	static int leftOld = GLFW_RELEASE;
	if (state == GLFW_PRESS && leftOld == GLFW_RELEASE)
	{
		PxSceneWriteLock lock(*g_PhysXManager->GetScene());


		GameObject* renderer = TestObject("renderer", (Mesh*)sphereMesh, nullptr, material, glm::vec3(0,0,0), 5);
		GameObject* rigid = new GameObject("ball");
		rigid->GetTransform()->AddChild(renderer->GetTransform(), false);
		rigid->GetTransform()->SetWorldPosition(camera->GetTransform()->GetWorldPosition() + camera->GetTransform()->GetForward() * 2);
		
		PxGeometry* geom = ResourceManager::GetInstance().GetResource<PxGeometry>("default_Sphere");
		RigidBody* rb = rigid->AddComponent<RigidBody>()->SetGeometry(geom, false);
		PxRigidDynamic* rigidDynamic = ((PxRigidDynamic*)rb->GetPxRigidActor());
		rigidDynamic->setMass(50);

		rigidDynamic->setLinearVelocity(GetPxVec3FromGLMVec3(camera->GetTransform()->GetForward()) * 220);
	}
	leftOld = state;

	state = glfwGetMouseButton(g_Window, GLFW_MOUSE_BUTTON_RIGHT);
	static int rightOld = GLFW_RELEASE;
	if (state == GLFW_PRESS && rightOld == GLFW_RELEASE)
	{
		float scale = 0.1f;
		GameObject* rootObject = new GameObject("char");
		rootObject->GetTransform()->SetLocalPosition(Camera::GetMainCamera()->GetTransform()->GetWorldPosition());
		rootObject->GetTransform()->SetLocalScale(glm::vec3(scale, scale, scale));

		Avatar* dupAvatar = Avatar::DuplicateAvatar(avatar);

		for (int i = 0; i < meshes.size(); ++i)
		{
			Mesh* mesh = dynamic_cast<Mesh*>(meshes[i]);
			if (mesh == nullptr)
				continue;
			GameObject* obj = nullptr;
			if (dynamic_cast<SkinnedMesh*>(meshes[i]) != nullptr)
				obj = TestObject("bone", (SkinnedMesh*)mesh, dupAvatar, skinnedMaterial, glm::vec3(0,0,0), 1);
			else
				obj = TestObject("boneMesh", mesh, nullptr, material, glm::vec3(0, 0, 0), 1);

			rootObject->GetTransform()->AddChild(obj->GetTransform(), false);
		}
		rootObject->GetTransform()->SetWorldPosition(camera->GetTransform()->GetWorldPosition() + camera->GetTransform()->GetForward() * 30);
		rootObject->GetTransform()->SetRotateWorld(camera->GetTransform()->GetWorldQuaternion());

		Ragdoll* ragdoll = rootObject->AddComponent<Ragdoll>();
		ragdoll->SetAvatar(dupAvatar);

		avatarRoot = dupAvatar->GetRoot();
		//root->SetWorldPosition(Camera::GetMainCamera()->GetTransform()->GetWorldPosition());

		RagdollInfo ragdollInfo;
		ragdollInfo.Head = avatarRoot->GetChildFromName("Character1_Head");
		ragdollInfo.LeftArm = avatarRoot->GetChildFromName("Character1_LeftArm");
		ragdollInfo.LeftElbow = avatarRoot->GetChildFromName("Character1_LeftForeArm");
		ragdollInfo.LeftFoot = avatarRoot->GetChildFromName("Character1_LeftFoot");
		ragdollInfo.LeftHips = avatarRoot->GetChildFromName("Character1_LeftUpLeg");
		ragdollInfo.LeftKnee = avatarRoot->GetChildFromName("Character1_LeftLeg");
		ragdollInfo.RightArm = avatarRoot->GetChildFromName("Character1_RightArm");
		ragdollInfo.RightElbow = avatarRoot->GetChildFromName("Character1_RightForeArm");
		ragdollInfo.RightFoot = avatarRoot->GetChildFromName("Character1_RightFoot");
		ragdollInfo.RightHips = avatarRoot->GetChildFromName("Character1_RightUpLeg");
		ragdollInfo.RightKnee = avatarRoot->GetChildFromName("Character1_RightLeg");
		ragdollInfo.MiddleSpine = avatarRoot->GetChildFromName("Character1_Spine1");
		ragdollInfo.Pelvis = avatarRoot->GetChildFromName("Character1_Hips");
		ragdoll->MakeRagdoll("TestRagdoll", ragdollInfo);
		//생성되는 GameObject전부 모아서 어케 해야것네.(Avatar의 BoneTransform 제어)
	}
	rightOld = state;

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
	if (avatarRoot == nullptr)
		return;
	std::stack<Transform*> transStack = std::stack<Transform*>();
	transStack.push(avatarRoot);

	float maxDist = 0;
	while (transStack.empty() == false)
	{
		auto parent = transStack.top();
		transStack.pop();

		for (int i = 0; i < parent->GetChildCount(); ++i)
		{
			auto child = parent->GetChild(i);

			Gizmo::SetColor(float((rand() % 255)) / 255.0f, float((rand() % 255)) / 255.0f, float((rand() % 255)) / 255.0f);
			Gizmo::DrawLine(camera, parent->GetWorldPosition(), child->GetWorldPosition());

			float dist = glm::distance(parent->GetWorldPosition(), child->GetWorldPosition());

			maxDist = glm::max(maxDist, dist);
			if (child->GetChildCount() > 0)
				transStack.push(child);
		}
	}

	Gizmo::SetColor(1,1,1);
}