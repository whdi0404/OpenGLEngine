#include "stdafx.h"
#include "Avatar.h"
#include "SkinnedMesh.h"

Avatar::Avatar()
{
}


Avatar::~Avatar()
{
}

Transform * Avatar::GetTransform(SkinnedMesh * skinnedMesh, std::string boneName)
{
	return skinnedMesh->GetBone(boneName);
}
