#pragma once
#include "PracticalRenderObject.h"
class SkinnedMeshRenderObject :
	public PracticalRenderObject
{
private:
	GetSetMacro(Avatar*, Avatar, avatar);
	static glm::mat4x4* skinnedMatrixBuffer;
	static int bufferSize;

public:
	SkinnedMeshRenderObject();
	~SkinnedMeshRenderObject();

	void Render(std::vector<PracticalRenderObject*>& renderObjects, Camera*& camera);
};

