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

void Avatar::AddNode(FbxNode * pNode)
{
	auto& iter = checkOverlapBuffer.find(pNode);
	if (iter == checkOverlapBuffer.end())
	{
		//CreateBone
		int boneIndex = checkOverlapBuffer.size();
		checkOverlapBuffer.insert(std::make_pair(pNode, boneIndex));

		Transform* boneTransform = new Transform();
		boneTransform->SetName(pNode->GetName());
		boneTransforms.push_back(boneTransform);
	}
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
		static std::unordered_set<FbxNode*> notAdded;
		auto& iter = checkOverlapBuffer.find(node);
		if (iter != checkOverlapBuffer.end())
		{
			FbxAMatrix deformationMatrix = FBXHelper::ComputeClusterDeformation(mesh, cluster);
			deformMatrices[iter->second] = GetGLMMatrixFromFBXMatrix(deformationMatrix);

			FbxAMatrix animationMatrix = FBXHelper::GetAnimationMatrix(cluster, 0, lPose);
			boneTransforms[iter->second]->SetName(node->GetName());
			boneTransforms[iter->second]->SetWorldMatrix(GetGLMMatrixFromFBXMatrix(animationMatrix));
		}
		else
		{
			if (notAdded.find(node) == notAdded.end())
			{
				FbxNodeAttribute *nodeAttributeFbx = node->GetNodeAttributeByIndex(0);
				FbxNodeAttribute::EType attributeType = nodeAttributeFbx->GetAttributeType();

				std::cout << "what the: " << node->GetName() << ", Attr: " << attributeType << std::endl;
				notAdded.insert(node);
			}
		}
	}
}

void Avatar::CalculateHierarchy()
{
	renderMatrices.resize(boneTransforms.size());
	deformMatrices.resize(boneTransforms.size());
	for (auto& iter : checkOverlapBuffer)
	{
		FbxNode* parentBone = iter.first->GetParent();

		if (parentBone != nullptr)
		{
			auto& parentIter = checkOverlapBuffer.find(parentBone);
			if (parentIter != checkOverlapBuffer.end())
				boneTransforms[iter.second]->SetParent(boneTransforms[parentIter->second], false);
			else
				root = boneTransforms[iter.second];
		}
	}
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
