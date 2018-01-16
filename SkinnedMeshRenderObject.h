#pragma once
#include "PracticalRenderObject.h"
class SkinnedMeshRenderObject :
	public PracticalRenderObject
{
private:
	GetSetMacro(Avatar*, Avatar, avatar);

public:
	SkinnedMeshRenderObject();
	~SkinnedMeshRenderObject();

	void Render(Camera*& camera);
};

