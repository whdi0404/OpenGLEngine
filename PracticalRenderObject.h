#pragma once
#include "RenderObject.h"

class Mesh;
class Material;
class Camera;
class PracticalRenderObject :
	public RenderObject
{
public:
	PracticalRenderObject();
	virtual ~PracticalRenderObject();

	GetMacro(Material*, Material, material);
	GetMacro(Mesh*, Mesh, mesh);
	GetMacro(uint32_t, HashCode, hashCode);

public:
	void SetMaterial(Material* material);

	void SetMesh(Mesh* mesh);

	virtual void Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera) = 0;
	void AddToVector();

private:
	void RefreshHashCode();
	std::vector<PracticalRenderObject*>* rendererVector;
};