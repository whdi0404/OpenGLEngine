#include "stdafx.h"
#include "Avatar.h"
#include "SkinnedMesh.h"
#include "FBXHelper.h"
#include "Time.h"

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

		Transform* boneTransform = new Transform(true);
		boneTransform->SetName(pNode->GetName());
		boneTransforms.emplace_back(boneTransform);
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

int Avatar::GetBoneIndexFromNode(FbxNode * node)
{
	auto& iter = checkOverlapBuffer.find(node);

	if (iter == checkOverlapBuffer.end())
		return -1;
	else
		return iter->second;
}

void Avatar::UpdateAnimation(KeyFrameAnimation* animation, float time)
{
	for (int i = 0; i < boneTransforms.size(); ++i)
		boneTransforms[i]->SetWorldMatrix(animation->GetMatrices(0, i, time));

	Update();
}

void Avatar::Update()
{
	for (int i = 0; i < renderMatrices.size(); ++i)
		renderMatrices[i] = boneTransforms[i]->GetWorldMatrix() * deformMatrices[i];
}

Avatar* Avatar::DuplicateAvatar(Avatar * avatar)
{
	Avatar* newAvatar = new Avatar();

	newAvatar->boneTransforms.reserve(avatar->boneTransforms.capacity());
	for (auto& transform : avatar->boneTransforms)
	{
		Transform* newTransform = new Transform();
		newTransform->SetName(transform->GetName());
		newTransform->SetWorldMatrix(transform->GetWorldMatrix());
		newAvatar->boneTransforms.emplace_back(newTransform);

		if (transform == avatar->root)
			newAvatar->root = newTransform;
	}

	for (int i = 0; i < avatar->boneTransforms.size(); ++i)
	{
		auto& transform = avatar->boneTransforms[i];
		if (transform->GetParent() != nullptr)
		{
			int parentIndex = std::find(avatar->boneTransforms.begin(), avatar->boneTransforms.end(), transform->GetParent()) - avatar->boneTransforms.begin();
			newAvatar->boneTransforms[i]->SetParent(newAvatar->boneTransforms[parentIndex]);
		}
	}

	newAvatar->deformMatrices = std::vector<glm::mat4x4>(avatar->deformMatrices);
	newAvatar->renderMatrices = std::vector<glm::mat4x4>(avatar->renderMatrices);

	return newAvatar;
}
