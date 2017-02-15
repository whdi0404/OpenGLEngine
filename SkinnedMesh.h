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
	void SetRoot(Transform* root);
	void AddBone(Transform* boneTransform);
	Transform* GetBone(std::string boneName);

public:
	virtual void DrawInstance(mat4x4* pMat, int count);

private:

	Avatar* avatar;
	std::unordered_map<std::string, Transform*> bones;
	std::vector<Transform*> boneTransforms;

	GetMacro(mat4x4*, BoneMatrices, boneMatrices);
	int GetBoneCount()
	{
		return boneTransforms.size();
	}
	GetMacro(Transform*, Root, root);
};