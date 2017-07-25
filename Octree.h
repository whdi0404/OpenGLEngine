#pragma once
#include "Object.h"

class Octree;
class RenderObject;
class GameObject;
class Camera;
class RendererObjectCollector;

using namespace Math;
class OctreeNode
{
	friend class Octree;

private:
	OctreeNode();
	OctreeNode(glm::vec3 center, glm::vec3 halfSize);

public:
	size_t GetNodeTreeDepth();

	uint32_t LocationCode;     // or 64-bit, depends on max. required tree depth
	uint8_t ChildExists;

	Box boundingBox;

	std::vector<GameObject*> list_RenderObjects;
	void AddNode(GameObject* gameObject);
};

class Octree :
	public Object
{
	static const int maxDepth;
public:
	enum Enum_Location
	{
		//1 = lft/rgt
		//2 = frt/bck
		//3 = btm/top
		Bottom_Left_Front = 0,	//000
		Bottom_Right_Front = 1,	//001
		Bottom_Left_Back = 2,	//010
		Bottom_Right_Back = 3,	//011
		Top_Left_Front = 4,		//100
		Top_Right_Front = 5,	//101
		Top_Left_Back = 6,		//110
		Top_Right_Back = 7		//111
	};

public:
	Octree(glm::vec3 scale, float looseWeight = 2);
	~Octree();

public:
	void AddObject(GameObject* object);
	void DeleteObject(GameObject* object);
	void ReallocateObject(GameObject* object);

	void GetCulledRenderObjects(RendererObjectCollector* renderObjCollector, Camera* camera);
	
	void GetChildBoundingBox(uint32_t parentLocCode, Enum_Location localLocCode, glm::vec3& center, glm::vec3& halfSize);

	void FindObject(std::string& objectName, std::vector<GameObject*>& objectList);

private:
	OctreeNode& GetCorrectNode(GameObject* object);

	void CreateCompleteOctree(uint32_t parentLocCode, int depth);
	void CollectAllRenderer(RendererObjectCollector* renderObjCollector, Camera* camera, OctreeNode& node);
	OctreeNode& GetParentNode(uint32_t locCode);
	OctreeNode& LookupNode(uint32_t locCode);
	OctreeNode& LookupNode(uint32_t parentLocCode, uint32_t locaLocCode);

	void AddNode(uint32_t parentLocationCode, uint8_t localLocationCode, bool linkChild = true);
	void DeleteNode(uint32_t locCode);

	static Enum_Location GetLocalLocCodeFromDirection(glm::vec3);

private:
	std::vector<OctreeNode> Nodes;
	glm::vec3 scale;
	float looseWeight;
	float invLooseWeight;

	GetMacro(int, ObjectCount, objectCount);
};