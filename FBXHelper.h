#pragma once
#include "KeyFrameAnimation.h"

class Mesh;
class SkinnedMesh;
class Object;
class VertexBuffer;
class Transform;
class Avatar;

class FBXHelper
{
public:
	static std::vector<Object*> GetResourcesFromFile(std::string filePath, glm::mat4x4 modelTransform);

private:
	static void LoadNode(std::vector<Object*>& refObject, FbxScene* fbxScene, FbxNode* fbxNode, glm::mat4x4 modelTransform, Avatar* avatar);

	static Mesh* LoadMeshData(FbxScene* fbxScene, FbxMesh *fbxMesh, glm::mat4x4 modelTransform, Avatar* avatar);

public :
	static FbxAMatrix ComputeClusterDeformation(FbxMesh* pMesh, FbxCluster* pCluster);

	static FbxAMatrix GetAnimationMatrix(FbxCluster* pCluster, FbxTime pTime, FbxPose* pPose);

	static FbxAMatrix GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose = nullptr);

	static FbxAMatrix GetPoseMatrix(FbxPose* pPose, int pNodeIndex);

	static FbxAMatrix GetGeometry(FbxNode* pNode);

	static void MatrixScale(FbxAMatrix& pMatrix, double pValue)
	{
		int i, j;

		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				pMatrix[i][j] *= pValue;
			}
		}
	}

	static void MatrixAddToDiagonal(FbxAMatrix& pMatrix, double pValue)
	{
		pMatrix[0][0] += pValue;
		pMatrix[1][1] += pValue;
		pMatrix[2][2] += pValue;
		pMatrix[3][3] += pValue;
	}

	static void MatrixAdd(FbxAMatrix& pDstMatrix, FbxAMatrix& pSrcMatrix)
	{
		int i, j;

		for (i = 0; i < 4; i++)
		{
			for (j = 0; j < 4; j++)
			{
				pDstMatrix[i][j] += pSrcMatrix[i][j];
			}
		}
	}

	static std::vector<KeyFrameAnimation*> LoadNodeKeyframeAnimation(FbxScene* fbxScene, std::map<FbxNode*, int>& boneNodes);
};