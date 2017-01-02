#pragma once
#include "PracticalRenderObject.h"

class Mesh;
class Material;
class MeshRenderObject :
	public PracticalRenderObject
{
public:
	MeshRenderObject();
	~MeshRenderObject();

protected:
	virtual void Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera);
};