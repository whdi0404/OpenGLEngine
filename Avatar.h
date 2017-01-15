#pragma once
#include "Object.h"
class SkinnedMesh;
class Avatar :
	public Object
{
private:
	std::unordered_set<std::string> boneNames;

public:
	Avatar();
	~Avatar();

	Transform* GetTransform(SkinnedMesh* skinnedMesh, std::string boneName);
};