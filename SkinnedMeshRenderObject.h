#pragma once
#include "PracticalRenderObject.h"
class SkinnedMeshRenderObject :
	public PracticalRenderObject
{
public:
	SkinnedMeshRenderObject();
	~SkinnedMeshRenderObject();

	void Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera);
};

