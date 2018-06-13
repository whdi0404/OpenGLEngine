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
	ResourceManager::GetInstance().SearchResources();
	InitResource();

	speed = 200.0f;
	rotSpeed = 120.0f;
	camera = (new GameObject())->AddComponent<Camera>();

	xyWeight = 1;
	octaves = 1;
	amplitude = 1;
	frequency = 1;
	h = 1;
	isFractal = false;

	//Shader* modelShader = new Shader("./Resources/Shaders/TestVertexShader.glsl", "./Resources/Shaders/TestFragmentShader.glsl");
	//Shader* skinnedShader = new Shader("./Resources/Shaders/TestSkinnedVertexShader.glsl", "./Resources/Shaders/TestFragmentShader.glsl");
	//
	//material = new Material(modelShader, 1);
	//skinnedMaterial = new Material(skinnedShader, 1);

	//Texture2D* modelTex = ResourceManager::GetInstance().GetResource<Texture2D>("Models/UnityChanTexture/body_01");
	//material->SetTexture(std::string("tex"), modelTex);
	//skinnedMaterial->SetTexture(std::string("tex"), modelTex);
	//
	//glm::mat4x4 modelMatrix = glm::mat4x4() * glm::axisAngleMatrix(glm::vec3(1, 0, 0), glm::radians(-90.0f));
	//modelMatrix = glm::rotate(modelMatrix, -glm::radians(90.0f), glm::vec3(0, 1, 0));

	//std::vector<Object*> meshes = FBXHelper::GetResourcesFromFile("./Models/SIG.FBX", modelMatrix);
	//TestObject((Mesh*)meshes[0], material, vec3(), 1);

	//meshes = ResourceManager::GetInstance().GetResourceAll<Object>(/*"./Models/unitychan.fbx"*/"Models/Maskboy");
	//avatar = (Avatar*)*std::find_if(meshes.begin(), meshes.end(), [](Object* obj) {return dynamic_cast<Avatar*>(obj) != nullptr; });
	//std::vector<Object*> animations = FBXHelper::GetResourcesFromFile("Models/Unitychan Animation/unitychan_RUN00_F.fbx", modelMatrix);//Unitychan Animation/unitychan_RUN00_F.fbx
	//gunMesh = ResourceManager::GetInstance().GetResourceAll<Object>("Models/Barrett.FBX");
	//sphereMesh = ResourceManager::GetInstance().GetResourceAll<Object>("Models/Sphere.FBX")[0];

	//keyFrameAnimation = (KeyFrameAnimation*)(*std::find_if(animations.begin(), animations.end(), [](Object* obj) -> bool
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

		Shader* tessellationShader = new Shader("./Resources/Shaders/TessVetexShader.glsl", "./Resources/Shaders/TessFragmentShader.glsl",
			"./Resources/Shaders/TessCtrlShader.glsl", "./Resources/Shaders/TessEvelShader.glsl", "./Resources/Shaders/TessGeomShader.glsl");
		float heightScale = 400.0f;
		float tileScale = 2.0f;

		//heightMap = ResourceManager::GetInstance().GetResource<Texture2D>("Tex/DinoIsland06.bmp"/**//*cDsYZ.jpg*/);
		heightSeed = ResourceManager::GetInstance().GetResource<Texture2D>("Tex/heighSeed.bmp");
		//Texture2D* normalMap = heightMap->CreateNormalTexture(heightMap, tileScale, heightScale);
		heightMap = Texture2D::CreateRandomHeightmap(2048, 2048, 6, 256.0f, heightSeed, nullptr, true);
		//Texture2D* normalMap = Texture2D::CreateNormalTexture(heightMap, 512, 512, tileScale, heightScale);
		resizeTexture = Texture2D::CreateNormalTexture(heightMap, 2048, 2048, tileScale, heightScale);
		//new Texture2D(heightMap->GetWidth(), heightMap->GetHeight(), Texture2D::DataType::Unsigned_Byte, Texture2D::Format::RGBA);
		terrainMaterial = new Material(tessellationShader, 1);
		terrainMaterial->SetTexture(std::string("heightMap"), heightMap);
		terrainMaterial->SetTexture(std::string("normalMap"), resizeTexture);
		//terrainMaterial->SetTexture(std::string("albedoMap"), albedoMap);

		terrainSystem->SetMaterial(terrainMaterial);
		terrainSystem->CreateMesh(heightMap, /*0.03125f*/tileScale, heightScale, 8);

		//obj->AddComponent<TerrainCollider>();

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
	//ResourceManager::GetInstance().AddResource("default_Material", default_Material);
	//
	//Mesh* box_Mesh = (Mesh*)FBXHelper::GetResourcesFromFile("./Models/Box.fbx")[0];
	//ResourceManager::GetInstance().AddPxConvexMeshGeometryResource("box_Mesh", box_Mesh, PxVec3(0.01f, 0.01f, 0.01f));

	//ResourceManager::GetInstance().AddPxSphereGeometryResource("default_Sphere", 2.5f);
	//ResourceManager::GetInstance().AddPxCapsuleGeometryResource("default_Capsule", 0.5f, 0.5f);
	//ResourceManager::GetInstance().AddPxBoxGeometryResource("default_Box", glm::vec3(0.5f, 0.5f, 0.5f));
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

	bool refresh = false;
	state = glfwGetKey(g_Window, GLFW_KEY_KP_7);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		xyWeight -= 1.0f * time;
		refresh = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_KP_8);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		xyWeight += 1.0f * time;
		refresh = true;
	}
	state = glfwGetKey(g_Window, GLFW_KEY_KP_4);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		octaves -= 1.0f * time;
		refresh = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_KP_5);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		octaves += 1.0f * time;
		refresh = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_KP_1);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		amplitude -= 1.0f * time;
		refresh = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_KP_2);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		amplitude += 1.0f * time;
		refresh = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_KP_6);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		frequency -= 1.0f * time;
		refresh = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_KP_9);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		frequency += 1.0f * time;
		refresh = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_KP_DIVIDE);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		h -= 1.0f * time;
		refresh = true;
	}

	state = glfwGetKey(g_Window, GLFW_KEY_KP_MULTIPLY);
	if (state == GLFW_PRESS)
	{
		float time = Time::GetInstance().GetDeltaTime();
		h += 1.0f * time;
		refresh = true;
	}


	state = glfwGetKey(g_Window, GLFW_KEY_T);
	if (state == GLFW_PRESS)
	{
		isFractal = !isFractal;
		refresh = true;
	} 


	if (refresh == true)
	{
		if (heightMap != nullptr)
		{
			delete heightMap;
			heightMap = nullptr;
		}
		heightMap = Texture2D::CreateRandomHeightmap(1024, 1024, 3, 100.0f, heightSeed, nullptr, isFractal);
		terrainMaterial->SetTexture(std::string("heightMap"), heightMap);
	}

	return;
	state = glfwGetMouseButton(g_Window, GLFW_MOUSE_BUTTON_LEFT);
	static int leftOld = GLFW_RELEASE;
	if (state == GLFW_PRESS && leftOld == GLFW_RELEASE)
	{
		//PxSceneWriteLock lock(*g_PhysXManager->GetScene());

		GameObject* renderer = TestObject("renderer", (Mesh*)sphereMesh, nullptr, material, /*glm::vec3(0,0,0)*/camera->GetTransform()->GetWorldPosition() + camera->GetTransform()->GetForward() * 2, 5);
		/*GameObject* rigid = new GameObject("ball");
		rigid->GetTransform()->AddChild(renderer->GetTransform(), false);
		rigid->GetTransform()->SetWorldPosition(camera->GetTransform()->GetWorldPosition() + camera->GetTransform()->GetForward() * 2);
		
		PxGeometry* geom = ResourceManager::GetInstance().GetResource<PxGeometry>("default_Sphere");
		RigidBody* rb = rigid->AddComponent<RigidBody>()->SetGeometry(geom, false);
		PxRigidDynamic* rigidDynamic = ((PxRigidDynamic*)rb->GetPxRigidActor());
		rigidDynamic->setMass(50);

		rigidDynamic->setLinearVelocity(GetPxVec3FromGLMVec3(camera->GetTransform()->GetForward()) * 220);*/
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
				obj = TestObject("bone", (SkinnedMesh*)mesh, dupAvatar, skinnedMaterial, glm::vec3(0, 0, 0), 1);
			else
				obj = TestObject("boneMesh", mesh, nullptr, material, glm::vec3(0, 0, 0), 1);

			rootObject->GetTransform()->AddChild(obj->GetTransform(), false);
		}
		rootObject->GetTransform()->SetWorldPosition(camera->GetTransform()->GetWorldPosition() + camera->GetTransform()->GetForward() * 30);
		rootObject->GetTransform()->SetRotateWorld(camera->GetTransform()->GetWorldQuaternion());

		/*Animator* animator = rootObject->AddComponent<Animator>();
		animator->SetAvatar(dupAvatar);
		animator->SetNowAnimation(keyFrameAnimation);*/

		//Ragdoll* ragdoll = rootObject->AddComponent<Ragdoll>();
		//ragdoll->SetAvatar(dupAvatar);
		//
		//avatarRoot = dupAvatar->GetRoot();
		////root->SetWorldPosition(Camera::GetMainCamera()->GetTransform()->GetWorldPosition());
		//
		//RagdollInfo ragdollInfo;
		//ragdollInfo.Head = avatarRoot->GetChildFromName("Character1_Head");
		//ragdollInfo.LeftArm = avatarRoot->GetChildFromName("Character1_LeftArm");
		//ragdollInfo.LeftElbow = avatarRoot->GetChildFromName("Character1_LeftForeArm");
		//ragdollInfo.LeftFoot = avatarRoot->GetChildFromName("Character1_LeftFoot");
		//ragdollInfo.LeftHips = avatarRoot->GetChildFromName("Character1_LeftUpLeg");
		//ragdollInfo.LeftKnee = avatarRoot->GetChildFromName("Character1_LeftLeg");
		//ragdollInfo.RightArm = avatarRoot->GetChildFromName("Character1_RightArm");
		//ragdollInfo.RightElbow = avatarRoot->GetChildFromName("Character1_RightForeArm");
		//ragdollInfo.RightFoot = avatarRoot->GetChildFromName("Character1_RightFoot");
		//ragdollInfo.RightHips = avatarRoot->GetChildFromName("Character1_RightUpLeg");
		//ragdollInfo.RightKnee = avatarRoot->GetChildFromName("Character1_RightLeg");
		//ragdollInfo.MiddleSpine = avatarRoot->GetChildFromName("Character1_Spine1");
		//ragdollInfo.Pelvis = avatarRoot->GetChildFromName("Character1_Hips");
		//ragdoll->MakeRagdoll("TestRagdoll", ragdollInfo);
		//생성되는 GameObject전부 모아서 어케 해야것네.(Avatar의 BoneTransform 제어)
	}
	rightOld = state;
	/*if (state == GLFW_PRESS)
	{
		auto& TimeManager = Time::GetInstance();
		double startTime = TimeManager.GetNowTimeSinceStart();
		for(int i = 0; i < 100; ++i)
			macMove500(stressTest);
		std::cout << "CalcTransform: " << (TimeManager.GetNowTimeSinceStart() - startTime) * 1000 << "ms" << std::endl;
	}*/
}