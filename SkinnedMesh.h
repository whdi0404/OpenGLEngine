#pragma once
#include "Mesh.h"
class Avatar;

class SkinnedMesh :
	public Mesh
{
public:
	SkinnedMesh();
	~SkinnedMesh();

	void SetAvatar(Avatar* avatar);
private:
	GetMacro(Avatar*, Avatar, avatar);
};