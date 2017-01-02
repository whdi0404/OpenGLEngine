#pragma once
#include "Transform.h"
#include "PracticalRenderObject.h"
#include "Material.h"
#include "Mesh.h"

class TestMesh;
class Texture2D;
class RenderObject;
class Camera;
class Material;
class Mesh;
class Octree;

class RendererObjectCollector
{
	struct RendererVector
	{
	private:
		RendererVector();
	public:
		RendererVector(PracticalRenderObject* renderObject)
		{
			renderOrder = renderObject->GetMaterial()->GetRenderOrder();
			resourceID = renderObject->GetMaterial()->GetRenderOrder();
			hashKey = renderObject->GetHashCode();
			renderobjects = new std::vector<PracticalRenderObject*>();
		}
		unsigned short renderOrder;
		unsigned short resourceID;
		uint32_t hashKey;
		std::vector<PracticalRenderObject*>* renderobjects;
	};
private:
	int keyID;
	std::unordered_map<int, int> renderObjectKey;

	Get(std::vector<Camera*>, CameraList, v_Camera);
	Get(std::vector<RendererVector>, RenderObjects, renderObjects);

public:
	RendererObjectCollector();
	~RendererObjectCollector();

	std::vector<PracticalRenderObject*>* RegisterRenderObject(RenderObject* renderObject);
	void AddDrawRenderObject(PracticalRenderObject* renderObject);
	void ReleaseRenderObject(RenderObject* renderObject);
	void ClearAllRenderObjects();
	void SortRenderObjects();

	static bool RenderSort(const RendererVector& first, const RendererVector& second);
};

class Renderer
{
public:
	Renderer();
	~Renderer();

	void Initialize();
	void Render(Octree* octree);
	void Release();

	Get(RendererObjectCollector*, Collector, renderObjectCollecdtor);
	std::vector<mat4x4> matrixBuffer;
	Material* prevMtrl = nullptr;
};

namespace std
{
	bool operator==(const std::string& x, const std::string& y);

	/*bool operator()(const MeshRenderObject::Key& x, const MeshRenderObject::Key& y) const
	{
		return x.material == y.material && x.mesh == y.mesh;
	}*/
}