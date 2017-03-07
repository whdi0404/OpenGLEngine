#pragma once
#include "Object.h"
class SkinnedMesh;
class Avatar :
	public Object
{
private:
	static std::map<FbxNode*, int> checkOverlapBuffer;

private:
	std::vector<mat4x4>  deformMatrices;
	std::vector<mat4x4>  renderMatrices;

	std::vector<Transform*> boneTransforms;
	GetMacro(Transform*, Root, root);

public:
	Avatar();
	~Avatar();

	//about FbxLoader
	void LoadMeshCluster(FbxScene* scene, FbxMesh* mesh, FbxSkin* skin);
	void CalculateHierarchy();
	int GetBoneIndexFromCluster(FbxCluster* cluster);

	void Update();

	mat4x4* GetRenderMatrices()
	{
		return renderMatrices.data();
	}

	int GetBoneCount()
	{
		return renderMatrices.size();
	}
};