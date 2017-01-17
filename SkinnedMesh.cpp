#include "stdafx.h"
#include "SkinnedMesh.h"

SkinnedMesh::SkinnedMesh()
{
}


SkinnedMesh::~SkinnedMesh()
{
}

void SkinnedMesh::SetAvatar(Avatar * avatar)
{
	this->avatar = avatar;
}

void SkinnedMesh::AddBone(Transform * boneTransform)
{
	boneArray.push_back(boneTransform);
	bones.insert(KEYVALUE(std::string, Transform*)(boneTransform->GetName(), boneTransform));
}

Transform * SkinnedMesh::GetBone(std::string boneName)
{
	auto pair = bones.find(boneName);
	if (pair != bones.end())
		return pair->second;

	return nullptr;
}
