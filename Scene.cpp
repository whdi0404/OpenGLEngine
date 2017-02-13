#include "stdafx.h"
#include "VertexBuffer.h"
#include "Scene.h"
#include "GameObject.h"
#include "MeshRenderObject.h"
#include "TerrainSystem.h"
#include "Material.h"
#include "Mesh.h"
#include "Camera.h"
#include "Shader.h"
#include "Time.h"
#include "Octree.h"
#include "Texture.h"
#include "SkinnedMesh.h"
//
//Scene::Scene()
//{
//	
//}
//
//Scene::~Scene()
//{
//}
//
//GameObject* TestObject(Mesh* mesh, Material* material, vec3 offset, float scale)
//{
//	GameObject* obj = new GameObject();
//	MeshRenderObject* meshRenderer = obj->AddComponent<MeshRenderObject>();
//
//	meshRenderer->SetMaterial(material);
//	meshRenderer->SetMesh(mesh);
//	obj->GetTransform()->SetLocalPosition(offset).SetLocalScale(scale, scale, scale).RotateAxisLocal(0, 0, 0);
//
//	return obj;
//}
//extern float maxScale;
//GameObject* TestTessellation(Texture2D* tex, Material* material, vec3 offset, float scale)
//{
//	GameObject* obj = new GameObject();
//	TerrainSystem* terrainSystem = obj->AddComponent<TerrainSystem>();
//
//	material->SetTexture(std::string("heightMap"), tex);
//	terrainSystem->SetMaterial(material);
//
//	terrainSystem->CreateMesh(tex, 1.f, 1);
//	obj->GetTransform()->SetLocalPosition(offset).SetLocalScale(scale, scale, scale).RotateAxisLocal(0, 0, 0);
//
//	return obj;
//}
//
//void Scene::Initialize()
//{
//	Shader* shader = new Shader("./Shaders/TestVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
//	Shader* skinnedShader = new Shader("./Shaders/TestSkinnedVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
//	Material* material = new Material(shader, 1);
//	Material* skinnedMaterial = new Material(skinnedShader, 1);
//
//	Texture2D* modelTex = new Texture2D("./Tex/Tex_0049_1.jpg");
//	material->SetTexture(std::string("tex"), modelTex);
//	skinnedMaterial->SetTexture(std::string("tex"), modelTex);
//	std::vector<Object*> meshes = FBXHelper::GetResourcesFromFile("./Models/SIG.FBX");
//
//	std::vector<Object*> bones = FBXHelper::GetResourcesFromFile("./Models/1 (1).fbx"); 
//	SkinnedMesh* skinnedMesh = dynamic_cast<SkinnedMesh*>(bones[0]);
//	TestObject((Mesh*)bones[0], skinnedMaterial, vec3(0,0,0), 1);
//
//	//CreateGizmos
//	{
//		VertexBuffer* vb = new VertexBuffer(TestVertexAttribute);
//		std::vector<int> indices;
//
//		Transform* root = skinnedMesh->GetRoot();
//
//		boneRoot = root;
//		std::queue<Transform*> traversalQueue;
//		traversalQueue.push(skinnedMesh->GetRoot());
//
//		/*vb->SetVertexCount(6);
//		vb->SetVector(Element::Position, 0, vec4(0, 0, 0, 1));
//		vb->SetVector(Element::Position, 1, vec4(100, 0, 0, 1));
//		vb->SetVector(Element::Position, 2, vec4(100, 0, 0, 1));
//		vb->SetVector(Element::Position, 3, vec4(100, 100, 0, 1));
//		vb->SetVector(Element::Position, 4, vec4(100, 100, 0, 1));
//		vb->SetVector(Element::Position, 5, vec4(0, 100, 0, 1));
//
//		indices.push_back(0);
//		indices.push_back(1);
//		indices.push_back(2);
//		indices.push_back(3);
//		indices.push_back(4);
//		indices.push_back(5);*/
//		int indexOrder = 0;
//
//		std::cout << root->GetName() << std::endl;
//		std::cout << "RootPos: x:" << root->GetWorldPosition().x << ", y:" << root->GetWorldPosition().y << ", z:" << root->GetWorldPosition().z << std::endl;
//
//		glm::vec3 maxS = glm::vec3();
//		glm::vec3 minS = glm::vec3(1000,1000,1000);
//		while (traversalQueue.empty() == false)
//		{
//			Transform* parent = traversalQueue.front();
//			traversalQueue.pop();
//
//			int childCnt = parent->GetChildCount();
//			vec3 parentPos = parent->GetWorldPosition();
//
//			for (int i = 0; i < childCnt; ++i)
//			{
//				Transform* childNode = parent->GetChild(i);
//
//				maxS = glm::max(maxS, childNode->GetLocalScale());
//				minS = glm::min(minS, childNode->GetLocalScale());
//				traversalQueue.push(childNode);
//
//				vec3 childPos = childNode->GetWorldPosition();
//
//				vb->SetVertexCount(indexOrder + 2);
//				vb->SetVector(Element::Position, indexOrder, vec4(parentPos.x, parentPos.y, parentPos.z, 1));
//				vb->SetVector(Element::Position, indexOrder + 1, vec4(childPos.x, childPos.y, childPos.z, 1));
//
//				indices.push_back(indexOrder);
//				indices.push_back(indexOrder + 1);
//				indexOrder += 2;
//
//				std::cout << "NAME: " << childNode->GetName() << ", PARENT: " << parent->GetName() << std::endl;
//				std::cout << "Pos: x:" << childPos.x << ", y:" << childPos.y << ", z:" << childPos.z << std::endl;
//			}
//		}
//		std::cout << "최소 x" << minS.x << ", y:" << minS.y << ", z:" << minS.z << std::endl;
//		std::cout << "최대 x" << maxS.x << ", y:" << maxS.y << ", z:" << maxS.z << std::endl;
//
//		gizmo = new Mesh();
//		gizmo->SetDrawMode(GL_LINES);
//		gizmo->SetMeshData(vb, indices);
//		TestObject(gizmo, material, glm::vec3(), 1);
//	}
//	
//	Shader* tessellationShader = new Shader("./Shaders/TessVetexShader.glsl", "./Shaders/TessFragmentShader.glsl", 
//		"./Shaders/TessCtrlShader.glsl", "./Shaders/TessEvelShader.glsl");
//	
//	terrainMaterial = new Material(tessellationShader, 1);
//
//	//std::vector<DefaultVertex> vertices;
//	//std::vector<int> indices;
//	//vertices.push_back(DefaultVertex(vec3(-1.0f, -1.0f, 0), vec2(0, 0)));
//	//vertices.push_back(DefaultVertex(vec3(1.0f, -1.0f, 0), vec2(1, 0)));
//	//vertices.push_back(DefaultVertex(vec3(1.0f, 1.0f, 0), vec2(1, 1)));
//	//vertices.push_back(DefaultVertex(vec3(-1.0f, 1.0f, 0), vec2(0, 1)));
//	
//	//indices.push_back(0);
//	//indices.push_back(1);
//	//indices.push_back(2);
//	//indices.push_back(0);
//	//indices.push_back(2);
//	//indices.push_back(3);
//
//	//Mesh* terrainMesh = new Mesh();
//	//terrainMesh->SetMeshData(vertices, indices);
//	
//
//	/*for (int x = 0; x < 10; ++x)
//		for (int y = 0; y < 10; ++y)*/
//	//for (int x = 0; x < 400; ++x)
//	//	for (int y = 0; y < 400; ++y)
//	//		TestTessellation(terrainMesh, terrainMaterial, vec3(x * 1 - 200, 0, y * 1 - 200), 1);
//
//	Texture2D* heightMap = new Texture2D("./Tex/1024x1024heightmap.png"/*cDsYZ.jpg*/);
//	//TestTessellation(heightMap, terrainMaterial, vec3(0, 0, 0), 1);
//
//	//TestObject(terrainMesh, terrainMaterial, vec3(), 1);
//
//	//for (int i = 0; i < meshes.size(); ++i)
//	//{
//	//	TestObject(meshes[i], material, vec3(100,10,-100));
//	//}
//	camera = (new GameObject())->AddComponent<Camera>();
//	camera->GetTransform()->SetLocalPosition(0, 0, 10);
//	camera->RefreshProjection();
//	camera->RefreshViewMatrix();
//}
//
//const static float speed = 4.0f;
//const static float rotSpeed = 120.0f;
//
//void Scene::Update()
//{
//	bool camChanged = false;
//	
//	float dt = Time::GetInstance().GetDeltaTime();
//	int state = glfwGetKey(g_Window, GLFW_KEY_W);
//	if (state == GLFW_PRESS)
//	{
//		if (glfwGetKey(g_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
//			boneRoot->SetLocalScale(boneRoot->GetLocalScale() * 0.9f);
//		else
//		{
//			camera->GetTransform()->AddLocalPosition(0, 0, -speed *dt);
//			camChanged = true;
//		}
//	}
//
//	state = glfwGetKey(g_Window, GLFW_KEY_S);
//	if (state == GLFW_PRESS)
//	{
//		camera->GetTransform()->AddLocalPosition(0, 0, speed *dt);
//		camChanged = true;
//	}
//
//	state = glfwGetKey(g_Window, GLFW_KEY_RIGHT);
//	if (state == GLFW_PRESS)
//	{
//		camera->GetTransform()->RotateAxisLocal(0, 0, -rotSpeed *dt);
//		camChanged = true;
//	}
//
//	state = glfwGetKey(g_Window, GLFW_KEY_LEFT);
//	if (state == GLFW_PRESS)
//	{
//		camera->GetTransform()->RotateAxisLocal(0, 0, rotSpeed *dt);
//		camChanged = true;
//	}
//
//	state = glfwGetKey(g_Window, GLFW_KEY_A);
//	if (state == GLFW_PRESS)
//	{
//		camera->GetTransform()->RotateAxisLocal(0, rotSpeed *dt, 0);
//		camChanged = true;
//	}
//
//	state = glfwGetKey(g_Window, GLFW_KEY_D);
//	if (state == GLFW_PRESS)
//	{
//		camera->GetTransform()->RotateAxisLocal(0, -rotSpeed *dt, 0);
//		camChanged = true;
//	}
//
//	state = glfwGetKey(g_Window, GLFW_KEY_DOWN);
//	if (state == GLFW_PRESS)
//	{
//		camera->GetTransform()->RotateAxisLocal(rotSpeed *dt, 0, 0);
//		camChanged = true;
//	}
//
//	state = glfwGetKey(g_Window, GLFW_KEY_UP);
//	if (state == GLFW_PRESS)
//	{
//		camera->GetTransform()->RotateAxisLocal(-rotSpeed * dt, 0, 0);
//		camChanged = true;
//	}
//
//	if (camChanged)
//	{
//		camera->RefreshViewMatrix();
//		glm::vec3 eyePos = camera->GetTransform()->GetWorldPosition();
//		//eyePos.y = 0;
//
//		terrainMaterial->SetVec3(std::string("eyePosition"), eyePos);
//	}
//}