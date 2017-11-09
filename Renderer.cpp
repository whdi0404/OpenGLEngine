#include "stdafx.h"
#include "Renderer.h"
#include "Texture.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "Mesh.h"
#include "Material.h"
#include "Time.h"
#include "Octree.h"

//#define DrawLine

Renderer::Renderer() : renderObjectCollecdtor(new RendererObjectCollector())
{
	matrixBuffer.reserve(1024);
}

Renderer::~Renderer()
{
}

void Renderer::Initialize()
{
	ilInit();

	glClearColor(0.6f, 0.8f, 0.85f, 0);
#ifdef DrawLine
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	glDisable(GL_CULL_FACE);
#else
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
#endif

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void Renderer::Render(Octree* octree)
{
	// 버퍼 지우기
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	std::vector<Camera*>& cameraList = renderObjectCollecdtor->GetCameraList();

	for (int i = 0; i < cameraList.size(); ++i)
	{
		Camera* cam = cameraList[i];
		cam->Update();

		prevMtrl = nullptr;
		renderObjectCollecdtor->ClearAllRenderObjects();
		octree->GetCulledRenderObjects(renderObjectCollecdtor, cam);

		auto& renderObjectsGroup = renderObjectCollecdtor->GetRenderObjects();
		for (int j = 0; j < renderObjectsGroup.size(); ++j)
		{
			auto& renderObjects = renderObjectsGroup[j];
			if (renderObjects.renderobjects->size() == 0)
				continue;

			/*for (int k = 0; k < renderObjects.renderobjects->size(); ++k)
				(*renderObjects.renderobjects)[k]->Update();*/
			
			matrixBuffer.clear();
			(*renderObjects.renderobjects)[0]->Render(*renderObjects.renderobjects, cam);
		}

	}
}

void Renderer::DrawGizmos()
{
	std::vector<Camera*>& cameraList = renderObjectCollecdtor->GetCameraList();

	for (int i = 0; i < cameraList.size(); ++i)
	{
		Camera* cam = cameraList[i];
		g_PhysXManager->DrawGizmos(cam);
	}
}

void Renderer::Release()
{
}

RendererObjectCollector::RendererObjectCollector() : keyID(0)
{
}

RendererObjectCollector::~RendererObjectCollector()
{
}

std::vector<PracticalRenderObject*>* RendererObjectCollector::RegisterRenderObject(RenderObject* renderObject)
{
	if (dynamic_cast<Camera*>(renderObject) != nullptr)
	{
		v_Camera.emplace_back((Camera*)renderObject);
		return nullptr;
	}
	else if (dynamic_cast<PracticalRenderObject*>(renderObject) != nullptr)
	{
		PracticalRenderObject* practicalRenderObject = (PracticalRenderObject*)renderObject;

		if (practicalRenderObject->GetMaterial() == nullptr || practicalRenderObject->GetMesh() == nullptr)
			return nullptr;

		unsigned int key = practicalRenderObject->GetHashCode();

		auto iter = renderObjectKey.find(key);
		if (iter == renderObjectKey.end())
		{
			auto keyValue = std::unordered_map<int, int>::value_type(key, keyID++);
			renderObjectKey.insert(keyValue);
			iter = renderObjectKey.find(key);

			RendererVector newRendererVector((PracticalRenderObject*)renderObject);
			renderObjects.emplace_back(newRendererVector);
			//std::vector<PracticalRenderObject*>* practicalRenderObjects = practicalRenderObjects = RendererVector((PracticalRenderObject*)renderObject).renderobjects;
			SortRenderObjects();
			return newRendererVector.renderobjects;
		}

		for (int i = 0; i < renderObjects.size(); ++i)
		{
			if (renderObjects[i].hashKey == practicalRenderObject->GetHashCode())
				return renderObjects[i].renderobjects;
		}
	}
	return nullptr;
}

void RendererObjectCollector::AddDrawRenderObject(PracticalRenderObject * renderObject)
{
	if (renderObject->GetHashCode() == -1)
		return;
}

void RendererObjectCollector::ReleaseRenderObject(RenderObject* renderObject)
{
	if (dynamic_cast<Camera*>(renderObject) != nullptr)
	{
		for (int i = 0; i < v_Camera.size(); ++i)
		{
			if (v_Camera[i] == renderObject)
			{	
				v_Camera.erase(v_Camera.begin() + i);
				break;
			}
		}
	}
	else if (dynamic_cast<PracticalRenderObject*>(renderObject) != nullptr)
	{
		PracticalRenderObject* practicalRenderObject = (PracticalRenderObject*)renderObject;

		if (practicalRenderObject->GetMaterial() == nullptr || practicalRenderObject->GetMesh() == nullptr)
			return;

		int key = practicalRenderObject->GetHashCode();

		auto iter = renderObjectKey.find(key);
		if (iter != renderObjectKey.end())
		{
			std::vector<PracticalRenderObject*>* practicalRenderers = renderObjects[iter->second].renderobjects;

			for (int i = 0; i < practicalRenderers->size(); ++i)
			{
				if ((*practicalRenderers)[i] == practicalRenderObject)
				{
					practicalRenderers->erase(practicalRenderers->begin() + i);
					break;
				}
			}
		}
	}
}

void RendererObjectCollector::ClearAllRenderObjects()
{
	for (int i = 0; i < renderObjects.size(); ++i)
	{
		renderObjects[i].renderobjects->clear();
	}
}

void RendererObjectCollector::SortRenderObjects()
{
	//새로운 메쉬/메테리얼이 추가될 떄만 소팅 함
	if (renderObjects.size() > 1)
		std::stable_sort(renderObjects.begin(), renderObjects.end(), RenderSort);
}

bool RendererObjectCollector::RenderSort(const RendererVector& first, const RendererVector& second)
{
	if (first.renderOrder != second.renderOrder)
		return first.renderOrder < second.renderOrder;

	return first.resourceID <  second.resourceID;
}

bool std::operator==(const std::string & x, const std::string & y)
{
	return x.compare(y) == 0;
}
