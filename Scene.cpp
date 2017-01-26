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

Scene::Scene()
{
	
}

Scene::~Scene()
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
extern float maxScale;
GameObject* TestTessellation(Texture2D* tex, Material* material, vec3 offset, float scale)
{
	GameObject* obj = new GameObject();
	TerrainSystem* terrainSystem = obj->AddComponent<TerrainSystem>();

	material->SetTexture(std::string("heightMap"), tex);
	terrainSystem->SetMaterial(material);

	terrainSystem->CreateMesh(tex, 1.f, 1);
	obj->GetTransform()->SetLocalPosition(offset).SetLocalScale(scale, scale, scale).RotateAxisLocal(0, 0, 0);

	return obj;
}

void Scene::Initialize()
{
	Shader* shader = new Shader("./Shaders/TestVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
	Shader* skinnedShader = new Shader("./Shaders/TestSkinnedVertexShader.glsl", "./Shaders/TestFragmentShader.glsl");
	Material* material = new Material(shader, 1);
	Material* skinnedMaterial = new Material(skinnedShader, 1);

	Texture2D* modelTex = new Texture2D("./Tex/Tex_0049_1.jpg");
	material->SetTexture(std::string("tex"), modelTex);
	skinnedMaterial->SetTexture(std::string("tex"), modelTex);
	std::vector<Object*> meshes = FBXHelper::GetResourcesFromFile("./Models/SIG.FBX");

	std::vector<Object*> bones = FBXHelper::GetResourcesFromFile("./Models/Chibi_Character_Up.FBX");
	SkinnedMesh* skinnedMesh = dynamic_cast<SkinnedMesh*>(bones[0]);
	//float offset = 5;
	gizmo = new Mesh();
	gizmo->SetDrawMode(GL_LINES);
	VertexBuffer* vb = new VertexBuffer(TestVertexAttribute);
	std::vector<int> indices;

	struct TestStructure { Transform* trans; GameObject* obj; };
	TestStructure root;
	root.trans = skinnedMesh->GetRoot();
	root.obj = TestObject((Mesh*)meshes[0], material, vec3(), 1.f);
	root.obj->GetTransform()->SetLocalMatrix(root.trans->GetLocalMatrix());
	
	boneRoot = root.obj->GetTransform();
	boneRoot->SetLocalScale(0.01f, 0.01f, 0.01f);
	boneRoot->SetName("root");
	std::queue<TestStructure> traversalQueue;
	traversalQueue.push(root);
	int indexOrder = 0;
	while (traversalQueue.empty() == false)
	{
		TestStructure parent = traversalQueue.front();
		traversalQueue.pop();

		int childCnt = parent.trans->GetChildCount();
		vec3 parentPos = parent.obj->GetTransform()->GetWorldPosition(); 
		for (int i = 0; i < childCnt; ++i)
		{
			Transform* childNode = parent.trans->GetChild(i);

			TestStructure structure;
			structure.trans = childNode;
			structure.obj = TestObject((Mesh*)meshes[0], material, vec3(), 1.0f);
			structure.obj->GetTransform()->SetParent(parent.obj->GetTransform(), false);
			structure.obj->GetTransform()->SetLocalMatrix(childNode->GetLocalMatrix());
			maxScale = max(maxScale, (float)structure.obj->GetTransform()->GetLossyScale().length());
			traversalQueue.push(structure);

			vec3 childPos = structure.obj->GetTransform()->GetWorldPosition();

			vb->SetVertexCount(indexOrder + 2);
			vb->SetVector(Element::Position, indexOrder, vec4(parentPos.x, parentPos.y, parentPos.z, 0));
			vb->SetVector(Element::Position, indexOrder + 1, vec4(childPos.x, childPos.y, childPos.z, 0));
			
			indices.push_back(indexOrder);
			indices.push_back(indexOrder + 1);
			indexOrder += 2;
		}
	}
	std::cout << maxScale << std::endl;

	gizmo->SetMeshData(vb, indices);
	TestObject(gizmo, material, glm::vec3(), 0.01f);
	TestObject((Mesh*)bones[0], skinnedMaterial, vec3(0,0,0), 0.01f);

	
	Shader* tessellationShader = new Shader("./Shaders/TessVetexShader.glsl", "./Shaders/TessFragmentShader.glsl", 
		"./Shaders/TessCtrlShader.glsl", "./Shaders/TessEvelShader.glsl");
	
	terrainMaterial = new Material(tessellationShader, 1);

	//std::vector<DefaultVertex> vertices;
	//std::vector<int> indices;
	//vertices.push_back(DefaultVertex(vec3(-1.0f, -1.0f, 0), vec2(0, 0)));
	//vertices.push_back(DefaultVertex(vec3(1.0f, -1.0f, 0), vec2(1, 0)));
	//vertices.push_back(DefaultVertex(vec3(1.0f, 1.0f, 0), vec2(1, 1)));
	//vertices.push_back(DefaultVertex(vec3(-1.0f, 1.0f, 0), vec2(0, 1)));
	
	//indices.push_back(0);
	//indices.push_back(1);
	//indices.push_back(2);
	//indices.push_back(0);
	//indices.push_back(2);
	//indices.push_back(3);

	//Mesh* terrainMesh = new Mesh();
	//terrainMesh->SetMeshData(vertices, indices);
	

	/*for (int x = 0; x < 10; ++x)
		for (int y = 0; y < 10; ++y)*/
	//for (int x = 0; x < 400; ++x)
	//	for (int y = 0; y < 400; ++y)
	//		TestTessellation(terrainMesh, terrainMaterial, vec3(x * 1 - 200, 0, y * 1 - 200), 1);

	Texture2D* heightMap = new Texture2D("./Tex/1024x1024heightmap.png"/*cDsYZ.jpg*/);
	//TestTessellation(heightMap, terrainMaterial, vec3(0, 0, 0), 1);

	//TestObject(terrainMesh, terrainMaterial, vec3(), 1);

	//for (int i = 0; i < meshes.size(); ++i)
	//{
	//	TestObject(meshes[i], material, vec3(100,10,-100));
	//}
	camera = (new GameObject())->AddComponent<Camera>();
	camera->GetTransform()->SetLocalPosition(0, 0, 10);
	camera->RefreshProjection();
	camera->RefreshViewMatrix();
}

const static float speed = 4.0f;
const static float rotSpeed = 120.0f;

void Scene::Update()
{
	bool camChanged = false;
	
	float dt = Time::GetInstance().GetDeltaTime();
	int state = glfwGetKey(g_Window, GLFW_KEY_W);
	if (state == GLFW_PRESS)
	{
		if (glfwGetKey(g_Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			boneRoot->SetLocalScale(boneRoot->GetLocalScale() * 0.9f);
		else
		{
			camera->GetTransform()->AddLocalPosition(0, 0, -speed *dt);
			camChanged = true;
		}
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

	if (camChanged)
	{
		camera->RefreshViewMatrix();
		glm::vec3 eyePos = camera->GetTransform()->GetWorldPosition();
		//eyePos.y = 0;

		terrainMaterial->SetVec3(std::string("eyePosition"), eyePos);
	}
		
}

/*#version 430 core
//fractional_even_spacing
layout (quads, fractional_odd_spacing,  ccw) in;
uniform mat4 matView;
uniform mat4 matProj;

uniform sampler2D heightMap;

in CS_OUT
{
	vec2 uv;
} es_in[];
out ES_OUT
{
	vec2 uv;
} es_out;

//gl_TessCoord.x, gl_TessCoord.y, gl_TessCoord.z에 세개 정점의 보간 정보가 들어있음.

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
   	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
	vec2 uv1 = mix(es_in[0].uv,es_in[1].uv,gl_TessCoord.x);
	vec2 uv2 = mix(es_in[2].uv,es_in[3].uv,gl_TessCoord.x);
	es_out.uv.yx = mix(uv1, uv2, gl_TessCoord.y);

	vec4 p1 = mix(gl_in[0].gl_Position,gl_in[1].gl_Position, gl_TessCoord.x);
	vec4 p2 = mix(gl_in[2].gl_Position,gl_in[3].gl_Position, gl_TessCoord.x);
	gl_Position = mix(p1, p2, gl_TessCoord.y);

	//gl_Position.y -= 0.5f;
	gl_Position.y = texture(heightMap, es_out.uv.xy).r;

	gl_Position = matProj * matView * gl_Position;
	}*/