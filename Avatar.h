#pragma once
#include "Object.h"
#include "KeyFrameAnimation.h"
class SkinnedMesh;
class Avatar :
	public Object
{
private:
	static std::map<FbxNode*, int> checkOverlapBuffer;

private:
	std::vector<glm::mat4x4>  deformMatrices;
	std::vector<glm::mat4x4>  renderMatrices;

	GetMacro(std::vector<Transform*>, BoneTrans, boneTransforms);
	GetMacro(Transform*, Root, root);

public:
	Avatar();
	~Avatar();

	//about FbxLoader

	void AddNode(FbxNode* pNode);
	void LoadMeshCluster(FbxScene* scene, FbxMesh* mesh, FbxSkin* skin);
	void CalculateHierarchy();
	int GetBoneIndexFromNode(FbxNode* fbxNode);

	void Update(KeyFrameAnimation* animation, float time);

	glm::mat4x4* GetRenderMatrices()
	{
		return renderMatrices.data();
	}

	int GetBoneCount()
	{
		return renderMatrices.size();
	}

	static std::vector<KeyFrameAnimation*> GetKeyFrameAnimations(FbxScene* scene, Avatar* avatar)
	{
		return FBXHelper::LoadNodeKeyframeAnimation(scene, avatar, checkOverlapBuffer);
	}
};