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
	void SetBone(Transform* root);
	void AddBone(Transform* boneTransform);
	Transform* GetBone(std::string boneName);

private:
	Avatar* avatar;
	std::unordered_map<std::string, Transform*> bones;
	std::vector<Transform*> boneArray;
	GetMacro(Transform*, Root, root);
};