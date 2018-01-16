#include "stdafx.h"
#include "GameObject.h"
#include "Transform.h"
#include "Octree.h"
#include "Camera.h"
#include "PracticalRenderObject.h"

const int Octree::maxDepth = 5;

Octree::Octree(glm::vec3 scale, float looseWeight)
{
	this->scale = scale;
	this->looseWeight = looseWeight;
	invLooseWeight = 1 / looseWeight;
	Nodes.resize(pow(8, maxDepth + 1) + 1, OctreeNode());
	AddNode(0, 1);
	CreateCompleteOctree(1, maxDepth);
}

Octree::~Octree()
{
}

OctreeNode& Octree::GetCorrectNode(GameObject* object)
{
	uint32_t targetLocCode = 1;

	float minLen = glm::min(glm::min(scale.x, scale.y), scale.z);
	int targetDepth = floor(log2(minLen) - log2(glm::max(0.001f, object->GetCullSphere().radius)));
	targetDepth = glm::min(targetDepth, maxDepth);

	Math::Sphere& objectSphere = object->GetCullSphere();

	for (int i = 0; i < targetDepth; ++i)
	{
		Math::Box& parentBox = LookupNode(targetLocCode).boundingBox;
		uint32_t localLoc = Octree::GetLocalLocCodeFromDirection(objectSphere.center - parentBox.center);

		AddNode(targetLocCode, localLoc);
		targetLocCode = (targetLocCode << 3) | localLoc;
	}

	return LookupNode(targetLocCode);
}

void Octree::AddObject(GameObject * object)
{
	GetCorrectNode(object).AddNode(object);
	++objectCount;
}

void Octree::DeleteObject(GameObject * object)
{
	OctreeNode& node = LookupNode(object->GetOctreeLocCode());
	if (node.LocationCode == 0)
		return;
	auto& renderObjects = node.list_RenderObjects;

	for (int i = 0; i < renderObjects.size(); ++i)
	{
		if (object == renderObjects[i])
		{
			renderObjects.erase(renderObjects.begin() + i);
			--objectCount;
			break;
		}
	}
	//삭제된 노드의 오브젝트 갯수가 0이면 삭제
	//if (renderObjects.size() == 0)
	//	DeleteNode(object->GetOctreeLocCode());
	object->SetOctreeLocCode(0);
}

void Octree::ReallocateObject(GameObject * object)
{
	OctreeNode& octreeNode = GetCorrectNode(object);
	if (object->GetOctreeLocCode() == octreeNode.LocationCode)
		return;
	else
	{
		DeleteObject(object);
		AddObject(object);
	}
}

void Octree::GetCulledRenderObjects(RendererObjectCollector* renderObjCollector, Camera* camera)
{
	CollectAllRenderer(renderObjCollector, camera, LookupNode(1));
}

void Octree::GetChildBoundingBox(uint32_t parentLocCode, Enum_Location localLocCode, glm::vec3 & center, glm::vec3 & halfSize)
{
	if (parentLocCode == 0)
	{
		center = glm::vec3(0, 0, 0);
		halfSize = this->scale * 0.5f * looseWeight;
	}
	else
	{
		OctreeNode& parentNode = this->LookupNode(parentLocCode);
		glm::vec3 parentTightHalfSize = parentNode.boundingBox.halfSize * invLooseWeight;

		switch (localLocCode)
		{
		case Enum_Location::Bottom_Left_Front:
			center = parentNode.boundingBox.center + (glm::vec3(-parentTightHalfSize.x, -parentTightHalfSize.y, -parentTightHalfSize.z) * 0.5f);
			halfSize = parentNode.boundingBox.halfSize * 0.5f;
			break;
		case Enum_Location::Top_Left_Front:
			center = parentNode.boundingBox.center + (glm::vec3(-parentTightHalfSize.x, parentTightHalfSize.y, -parentTightHalfSize.z) * 0.5f);
			halfSize = parentNode.boundingBox.halfSize * 0.5f;
			break;
		case Enum_Location::Bottom_Right_Front:
			center = parentNode.boundingBox.center + (glm::vec3(parentTightHalfSize.x, -parentTightHalfSize.y, -parentTightHalfSize.z) * 0.5f);
			halfSize = parentNode.boundingBox.halfSize * 0.5f;
			break;
		case Enum_Location::Top_Right_Front:
			center = parentNode.boundingBox.center + (glm::vec3(parentTightHalfSize.x, parentTightHalfSize.y, -parentTightHalfSize.z) * 0.5f);
			halfSize = parentNode.boundingBox.halfSize * 0.5f;
			break;
		case Enum_Location::Bottom_Left_Back:
			center = parentNode.boundingBox.center + (glm::vec3(-parentTightHalfSize.x, -parentTightHalfSize.y, parentTightHalfSize.z) * 0.5f);
			halfSize = parentNode.boundingBox.halfSize * 0.5f;
			break;
		case Enum_Location::Top_Left_Back:
			center = parentNode.boundingBox.center + (glm::vec3(-parentTightHalfSize.x, parentTightHalfSize.y, parentTightHalfSize.z) * 0.5f);
			halfSize = parentNode.boundingBox.halfSize * 0.5f;
			break;
		case Enum_Location::Bottom_Right_Back:
			center = parentNode.boundingBox.center + (glm::vec3(parentTightHalfSize.x, -parentTightHalfSize.y, parentTightHalfSize.z) * 0.5f);
			halfSize = parentNode.boundingBox.halfSize * 0.5f;
			break;
		case Enum_Location::Top_Right_Back:
			center = parentNode.boundingBox.center + (glm::vec3(parentTightHalfSize.x, parentTightHalfSize.y, parentTightHalfSize.z) * 0.5f);
			halfSize = parentNode.boundingBox.halfSize * 0.5f;
			break;
		}
	}
}

void Octree::CreateCompleteOctree(uint32_t parentLocCode, int depth)
{
	for (int i = 0; i < 8; ++i)
	{
		AddNode(parentLocCode, i, false);

		if (depth > 0)
			CreateCompleteOctree(parentLocCode << 3 | i, depth - 1);
	}
}

void Octree::CollectAllRenderer(RendererObjectCollector* renderObjCollector, Camera* camera, OctreeNode& node)
{
	if (camera->frustum.boxInFrustum(node.boundingBox) == FrustumG::OUTSIDE)
	{
		return;
	}
	else
	{
		for (int i = 0; i < node.list_RenderObjects.size(); ++i)
		{
			auto& obj = node.list_RenderObjects[i];
			if (dynamic_cast<PracticalRenderObject*>(obj->renderObject) != nullptr)
			{
				auto& cullSphere = obj->GetCullSphere();
				if (camera->frustum.sphereInFrustum(cullSphere) != FrustumG::OUTSIDE)
					((PracticalRenderObject*)node.list_RenderObjects[i]->renderObject)->AddToVector();
				else
				{
					int a = 0;
					a++;
					//std::cout << "" << std::endl;
				}
			}
		}
	}

	for (int i = 0; i < 8; i++)
	{
		if (node.ChildExists&(1 << i))
		{
			uint32_t locCodeChild = (node.LocationCode << 3) | i;
			OctreeNode& child = LookupNode(locCodeChild);
			CollectAllRenderer(renderObjCollector, camera, child);
		}
	}
}

OctreeNode& Octree::GetParentNode(uint32_t locNode)
{
	return LookupNode(locNode >> 3);
}

OctreeNode& Octree::LookupNode(uint32_t locCode)
{
	if (Nodes.size() <= locCode)
		return OctreeNode();
	return Nodes[locCode];
}

OctreeNode& Octree::LookupNode(uint32_t parentLocCode, uint32_t locaLocCode)
{
	return LookupNode(parentLocCode << 3 | locaLocCode);
}

void Octree::AddNode(uint32_t parentLocationCode, uint8_t localLocationCode, bool linkChild)
{

	uint32_t newNodeLocCode = (parentLocationCode << 3) | localLocationCode;
	if (Nodes.size() <= newNodeLocCode)
	{
		Nodes.resize(newNodeLocCode + 1, OctreeNode());
	}
	OctreeNode& parentNode = this->LookupNode(parentLocationCode);

	if (Nodes[newNodeLocCode].LocationCode == 0)
	{
		GetChildBoundingBox(parentLocationCode, (Enum_Location)localLocationCode, Nodes[newNodeLocCode].boundingBox.center, Nodes[newNodeLocCode].boundingBox.halfSize);
		Nodes[newNodeLocCode].LocationCode = newNodeLocCode;
	}

	if (linkChild && parentNode.LocationCode != 0)
	{
		parentNode.ChildExists |= 1 << localLocationCode;
	}
}

void Octree::DeleteNode(uint32_t locCode)
{
	OctreeNode& parent = GetParentNode(locCode);
	if (parent.LocationCode == 0)
		return;

	uint8_t localLocationMask = ~(1 >> ((locCode << 29) >> 29));
	parent.ChildExists &= localLocationMask;

	if (parent.ChildExists == 0 && parent.list_RenderObjects.size() == 0)
	{
		DeleteNode(parent.LocationCode);
	}
}

Octree::Enum_Location Octree::GetLocalLocCodeFromDirection(glm::vec3 dir)
{
	//1 = lft/rgt
	//2 = frt/bck
	//3 = btm/top
	uint32_t x = dir.x > 0;
	uint32_t z = dir.z > 0;
	uint32_t y = dir.y > 0;

	return Enum_Location(x | z << 1 | y << 2);
}

void Octree::FindObject(std::string& objectName, std::vector<GameObject*>& objectList)
{
	for(auto& octreeNode : Nodes)
		for(auto obj : octreeNode.list_RenderObjects)
			if(objectName == obj->GetName())
				objectList.emplace_back(obj);
}

OctreeNode::OctreeNode()
{
	LocationCode = 0;
	ChildExists = 0;
}

OctreeNode::OctreeNode(glm::vec3 center, glm::vec3 halfSize)
{
	ChildExists = 0;
	this->boundingBox.center = center;
	this->boundingBox.halfSize = halfSize;
}

size_t OctreeNode::GetNodeTreeDepth()
{
	assert(LocationCode);

	unsigned long msb;
	_BitScanReverse(&msb, LocationCode);
	return msb / 3;
}

void OctreeNode::AddNode(GameObject * gameObject)
{
	list_RenderObjects.emplace_back(gameObject);
	gameObject->SetOctreeLocCode(LocationCode);
}