#include "stdafx.h"
#include "Avatar.h"
#include "SkinnedMesh.h"
#include "FBXHelper.h"

std::map<FbxNode*, int> Avatar::checkOverlapBuffer = std::map<FbxNode*, int>();

Avatar::Avatar()
{
	checkOverlapBuffer.clear();
}

Avatar::~Avatar()
{
}

void Avatar::LoadMeshCluster(FbxScene* scene, FbxMesh* mesh, FbxSkin* skin)
{
	FbxPose* lPose = scene->GetPose(0);

	int boneCount = skin->GetClusterCount();

	for (int i = 0; i < boneCount; ++i)
	{
		FbxCluster* cluster = skin->GetCluster(i);
		FbxNode* node = cluster->GetLink();
		if (node == nullptr)
			continue;

		auto& iter = checkOverlapBuffer.find(node);
		if (iter == checkOverlapBuffer.end())
		{
			//CreateBone
			int boneIndex = checkOverlapBuffer.size();
			checkOverlapBuffer.insert(std::make_pair(node, boneIndex));

			FbxAMatrix deformationMatrix = FBXHelper::ComputeClusterDeformation(mesh, cluster);
			deformMatrices.push_back(GetGLMMatrixFromFBXMatrix(deformationMatrix));

			FbxAMatrix animationMatrix = FBXHelper::GetAnimationMatrix(cluster, 0, lPose);
			Transform* boneTransform = new Transform();
			boneTransform->SetName(node->GetName());
			boneTransform->SetLocalMatrix(GetGLMMatrixFromFBXMatrix(animationMatrix));

			boneTransforms.push_back(boneTransform);
		}
	}
}

void Avatar::CalculateHierarchy()
{
	renderMatrices.resize(deformMatrices.size());

	for (auto& iter : checkOverlapBuffer)
	{
		FbxNode* parentBone = iter.first->GetParent();

		if (parentBone != nullptr)
		{
			auto& parentIter = checkOverlapBuffer.find(parentBone);
			if (parentIter != checkOverlapBuffer.end())
				boneTransforms[iter.second]->SetParent(boneTransforms[parentIter->second]);
			else
				root = boneTransforms[iter.second];
		}
	}
	Update();

	checkOverlapBuffer.clear();
}

int Avatar::GetBoneIndexFromCluster(FbxCluster * cluster)
{
	auto& iter = checkOverlapBuffer.find(cluster->GetLink());

	if (iter == checkOverlapBuffer.end())
		return -1;
	else
		return iter->second;
}

void Avatar::Update()
{
	for (int i = 0; i < renderMatrices.size(); ++i)
		renderMatrices[i] = boneTransforms[i]->GetWorldMatrix() * deformMatrices[i];
}
