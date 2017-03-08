#include "stdafx.h"
#include "FBXHelper.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "VertexBuffer.h"
#include "MeshVertexAttribute.h"
#include "Avatar.h"
#include "KeyFrameAnimation.h"

std::vector<Object*> FBXHelper::GetResourcesFromFile(std::string filePath, glm::mat4x4 modelTransform)
{
	std::stack<FbxNode*> stack;
	if (g_FbxSdkManager == nullptr)
	{
		g_FbxSdkManager = FbxManager::Create();
		FbxIOSettings* pIOsettings = FbxIOSettings::Create(g_FbxSdkManager, IOSROOT);
		g_FbxSdkManager->SetIOSettings(pIOsettings);
	}

	std::vector<Object*> returnObject;
	FbxImporter* pImporter = FbxImporter::Create(g_FbxSdkManager, "");
	FbxScene* pFbxScene = FbxScene::Create(g_FbxSdkManager, "");

	if (pImporter->Initialize(filePath.c_str(), -1, g_FbxSdkManager->GetIOSettings()))
	{
		if (pImporter->Import(pFbxScene))
		{
			Avatar* avatar = new Avatar();

			FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
			stack.push(pFbxRootNode);
			while (stack.empty() == false)
			{
				FbxNode* node = stack.top();
				stack.pop();

				for (int i = 0; i < node->GetChildCount(); ++i)
				{
					FbxNode* childNode = node->GetChild(i);

					LoadNode(returnObject, pFbxScene, childNode, modelTransform, avatar);

					if (childNode->GetChildCount() != 0)
						stack.push(childNode);
				}
			}

			avatar->CalculateHierarchy(pFbxScene);
			if (avatar->GetBoneCount() == 0)
			{
				delete avatar;
			}

			pFbxRootNode->Destroy();
		}
	}

	pImporter->Destroy();
	pFbxScene->Destroy();

	return returnObject;
}

void FBXHelper::LoadNode(std::vector<Object*>& refObject, FbxScene* fbxScene, FbxNode* fbxNode, glm::mat4x4 modelTransform, Avatar* avatar)
{
	int numAttributes = fbxNode->GetNodeAttributeCount();
	for (int i = 0; i < numAttributes; i++)
	{
		FbxNodeAttribute *nodeAttributeFbx = fbxNode->GetNodeAttributeByIndex(i);
		FbxNodeAttribute::EType attributeType = nodeAttributeFbx->GetAttributeType();

		switch (attributeType)
		{
		case FbxNodeAttribute::eMesh:
		{
			FbxMesh* fbxMesh = (FbxMesh*)nodeAttributeFbx;

			Mesh* mesh = LoadMeshData(fbxScene, (FbxMesh*)nodeAttributeFbx, modelTransform, avatar);
			if (mesh == nullptr)
				continue;

			refObject.push_back(mesh);
		}

		break;
		}
	}
}

Mesh* FBXHelper::LoadMeshData(FbxScene* fbxScene, FbxMesh *fbxMesh, glm::mat4x4 modelTransform, Avatar* avatar)
{
	std::unordered_map<int, std::vector<int>> duplicatedVertex;

	VertexBuffer* vertexBuffer;
	std::vector<int> indices;

	std::vector<vec3> position;
	std::vector<vec2> uvs;

	int vertexCount = fbxMesh->GetControlPointsCount();

	uvs.resize(vertexCount);

	FbxLayerElementArrayTemplate<FbxVector2>* pTexUvs;
	fbxMesh->GetTextureUV(&pTexUvs);
	if (pTexUvs == nullptr)
		return nullptr;

	FbxLayerElementUV* layerUVs = fbxMesh->GetLayer(0)->GetUVs();
	FbxLayerElement::EMappingMode mappingMode = layerUVs->GetMappingMode();
	FbxLayerElement::EReferenceMode referenceMode = layerUVs->GetReferenceMode();

	FbxVector4* pVertices = fbxMesh->GetControlPoints();
	for (int j = 0; j < vertexCount; ++j)
	{
		vec3 vertex;
		vertex.x = (float)pVertices[j].mData[0];
		vertex.y = (float)pVertices[j].mData[1];
		vertex.z = (float)pVertices[j].mData[2];

		position.push_back(vertex);

		if (mappingMode == FbxLayerElement::eByControlPoint)
		{
			int uvIndex = 0;
			if (referenceMode == FbxLayerElement::eDirect)
				uvIndex = j;
			else if (referenceMode == FbxLayerElement::eIndexToDirect)
				uvIndex = layerUVs->GetIndexArray().GetAt(j);
			uvs[uvIndex].x = (*pTexUvs)[uvIndex].mData[0];
			uvs[uvIndex].y = (*pTexUvs)[uvIndex].mData[1];
		}
	}

	int polygonCount = fbxMesh->GetPolygonCount();
	for (int j = 0; j < polygonCount; j++)
	{
		int iNumVertices = fbxMesh->GetPolygonSize(j);

		bool duplicated = false;
		std::vector<int> realIndices;
		if (mappingMode == FbxLayerElement::eByPolygonVertex)
		{
			for (int k = 0; k < iNumVertices; k++)
			{
				int iControlPointIndex = fbxMesh->GetPolygonVertex(j, k);
				int uvIndex = 0;
				if (referenceMode == FbxLayerElement::eDirect ||
					referenceMode == FbxLayerElement::eIndexToDirect)
					uvIndex = fbxMesh->GetTextureUVIndex(j, k);

				vec2 uv;
				uv.x = (*pTexUvs)[uvIndex].mData[0];
				uv.y = (*pTexUvs)[uvIndex].mData[1];

				auto& iter = duplicatedVertex.find(iControlPointIndex);
				if (iter == duplicatedVertex.end())
				{
					duplicatedVertex.insert(std::make_pair(iControlPointIndex, std::vector<int>()));
					uvs[iControlPointIndex] = uv;
					realIndices.push_back(iControlPointIndex);
				}
				else
				{
					int newVertexIndex = position.size();
					iter->second.push_back(newVertexIndex);
					position.push_back(position[iControlPointIndex]);
					uvs.push_back(uv);
					realIndices.push_back(newVertexIndex);
					duplicated = true;
				}
			}
		}

		if (iNumVertices == 3)
		{
			indices.push_back(realIndices[0]);
			indices.push_back(realIndices[1]);
			indices.push_back(realIndices[2]);
		}

		if (iNumVertices == 4)
		{
			indices.push_back(realIndices[0]);
			indices.push_back(realIndices[1]);
			indices.push_back(realIndices[2]);

			indices.push_back(realIndices[0]);
			indices.push_back(realIndices[2]);
			indices.push_back(realIndices[3]);
		}
	}

	int numDeformers = fbxMesh->GetDeformerCount();
	FbxSkin* skin = (FbxSkin*)fbxMesh->GetDeformer(0, FbxDeformer::eSkin);

	if (skin != nullptr && skin->GetClusterCount() != 0)
	{
		SkinnedMesh* newMesh = new SkinnedMesh();
		newMesh->SetAvatar(avatar);

		std::vector<vec4> boneWeights;
		std::vector<vec4> boneIndices;
		boneWeights.resize(position.size(), vec4());
		boneIndices.resize(position.size(), vec4());

		avatar->LoadMeshCluster(fbxScene, fbxMesh, skin);

		int boneCount = skin->GetClusterCount();
		for (int boneIndex = 0; boneIndex < boneCount; boneIndex++)
		{
			FbxCluster* cluster = skin->GetCluster(boneIndex);

			int *boneVertexIndices = cluster->GetControlPointIndices();
			int numBoneVertexIndices = cluster->GetControlPointIndicesCount();
			double *boneVertexWeights = cluster->GetControlPointWeights();
			int avatarBoneIndex = avatar->GetBoneIndexFromCluster(cluster);

			for (int i = 0; i < numBoneVertexIndices; i++)
			{
				int boneVertexIndex = boneVertexIndices[i];

				for (int element = 0; element < 4; ++element)
				{
					if (boneWeights[boneVertexIndex][element] == 0)
					{
						boneIndices[boneVertexIndex][element] = avatarBoneIndex;
						boneWeights[boneVertexIndex][element] = boneVertexWeights[i];
						break;
					}
				}
			}
		}
		std::for_each(boneWeights.begin(), boneWeights.end(), [](vec4& v)
		{
			if (v.x == -1) v.x = 0;
			if (v.y == -1) v.y = 0;
			if (v.z == -1) v.z = 0;
			if (v.w == -1) v.w = 0;

			v /= (v.x + v.y + v.z + v.w);
		});
		//UV, Indexing

		vertexBuffer = new VertexBuffer(SkinningVertexAttribute);
		vertexBuffer->SetVertexCount(position.size());

		for (auto& iter : duplicatedVertex)
		{
			if (iter.second.size() > 0)
			{
				for (auto& iter2 : iter.second)
				{
					boneWeights[iter2] = boneWeights[iter.first];
					boneIndices[iter2] = boneIndices[iter.first];
				}
			}
		}

		for (int i = 0; i < position.size(); ++i)
		{
			vertexBuffer->SetVector(Element::Position, i, glm::vec4(position[i].x, position[i].y, position[i].z, 0));
			vertexBuffer->SetVector(Element::Texcoord0, i, glm::vec4(uvs[i].x, uvs[i].y, 0, 0));
			vertexBuffer->SetVector(Element::BoneWeights, i, boneWeights[i]);
			vertexBuffer->SetVector(Element::BoneIndices, i, boneIndices[i]);
		}
		newMesh->SetMeshData(vertexBuffer, indices);
		return newMesh;
	}
	else
	{
		vertexBuffer = new VertexBuffer(TestVertexAttribute);
		vertexBuffer->SetVertexCount(position.size());
		for (int i = 0; i < position.size(); ++i)
		{
			vertexBuffer->SetVector(Element::Position, i, glm::vec4(position[i].x, position[i].y, position[i].z, 0));
			vertexBuffer->SetVector(Element::Texcoord0, i, glm::vec4(uvs[i].x, uvs[i].y, 0, 0));
		}
		Mesh* newMesh = new Mesh();
		newMesh->SetMeshData(vertexBuffer, indices);
		return newMesh;
	}

	return nullptr;
}

FbxAMatrix FBXHelper::ComputeClusterDeformation(FbxMesh* pMesh, FbxCluster* pCluster)
{
	FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();

	if (lClusterMode == FbxCluster::eNormalize)
	{
		FbxAMatrix lReferenceGlobalInitPosition;
		FbxAMatrix lClusterGlobalInitPosition;
		FbxAMatrix lReferenceGeometry;

		pCluster->GetTransformMatrix(lReferenceGlobalInitPosition);

		lReferenceGeometry = GetGeometry(pMesh->GetNode());
		lReferenceGlobalInitPosition *= lReferenceGeometry;

		pCluster->GetTransformLinkMatrix(lClusterGlobalInitPosition);

		return lClusterGlobalInitPosition.Inverse() * lReferenceGlobalInitPosition;
	}
	else
	{
		std::cout << "Not eNormalize - ComputeClusterDeformation " << std::endl;
		return FbxAMatrix();
	}
}
FbxAMatrix FBXHelper::GetAnimationMatrix(FbxCluster* pCluster, FbxTime pTime, FbxPose* pPose)
{
	FbxCluster::ELinkMode lClusterMode = pCluster->GetLinkMode();
	if (lClusterMode == FbxCluster::eNormalize)
	{
		return GetGlobalPosition(pCluster->GetLink(), pTime, pPose);
	}
	else
	{
		std::cout << "Not eNormalize - GetAnimationMatrix " << std::endl;
		return FbxAMatrix();
	}
}

FbxAMatrix  FBXHelper::GetGlobalPosition(FbxNode* pNode, const FbxTime& pTime, FbxPose* pPose)
{
	FbxAMatrix lGlobalPosition;
	bool        lPositionFound = false;

	if (pPose)
	{
		int lNodeIndex = pPose->Find(pNode);

		if (lNodeIndex > -1)
		{
			lGlobalPosition = GetPoseMatrix(pPose, lNodeIndex);
			lPositionFound = true;
		}
	}

	if (!lPositionFound)
	{
		lGlobalPosition = pNode->EvaluateGlobalTransform(pTime);
	}

	return lGlobalPosition;
}

FbxAMatrix FBXHelper::GetPoseMatrix(FbxPose* pPose, int pNodeIndex)
{
	FbxAMatrix lPoseMatrix;
	FbxMatrix lMatrix = pPose->GetMatrix(pNodeIndex);

	memcpy((double*)lPoseMatrix, (double*)lMatrix, sizeof(lMatrix.mData));

	return lPoseMatrix;
}

FbxAMatrix FBXHelper::GetGeometry(FbxNode* pNode)
{
	const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

std::vector<KeyFrameAnimation*> FBXHelper::LoadNodeKeyframeAnimation(FbxScene* fbxScene, std::map<FbxNode*, int>& boneNodes)
{
	std::vector<KeyFrameAnimation*> animations;
	FbxCriteria condi = FbxCriteria::ObjectTypeStrict(FbxAnimStack::ClassId);
	int numAnimations = fbxScene->GetSrcObjectCount(condi);

	for (int animationIndex = 0; animationIndex < numAnimations; animationIndex++)
	{
		FbxAnimStack *animStack = (FbxAnimStack*)(fbxScene->GetSrcObject(condi, animationIndex));
		FbxAnimEvaluator *animEvaluator = fbxScene->GetAnimationEvaluator();

		KeyFrameAnimation* newAnimation = new KeyFrameAnimation();
		newAnimation->SetName(animStack->GetName());
		
		int numLayers = animStack->GetMemberCount();

		for (auto& boneIter : boneNodes)
		{
			FbxNode* fbxNode = boneIter.first;

			//아직 레이어는 작업 안 됨
			for (int layerIndex = 0; layerIndex < numLayers; layerIndex++)
			{
				FbxAnimLayer *animLayer = (FbxAnimLayer*)animStack->GetMember(layerIndex);

				FbxAnimCurve *translationCurve = fbxNode->LclTranslation.GetCurve(animLayer);
				FbxAnimCurve *rotationCurve = fbxNode->LclRotation.GetCurve(animLayer);
				FbxAnimCurve *scalingCurve = fbxNode->LclScaling.GetCurve(animLayer);
				{
					if (scalingCurve == nullptr)
					{
						FbxDouble3 scalingVector = fbxNode->LclScaling.Get();
						float x = (float)scalingVector[0];
						float y = (float)scalingVector[1];
						float z = (float)scalingVector[2];
						newAnimation->AddKeyFrame(layerIndex, boneIter.second, new KeyFrameScaling(glm::vec3(x, y, z), 0));
					}
					else
					{
						int numKeys = scalingCurve->KeyGetCount();
						for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
						{
							FbxTime frameTime = scalingCurve->KeyGetTime(keyIndex);
							FbxDouble3 scalingVector = fbxNode->EvaluateLocalScaling(frameTime);
							float x = (float)scalingVector[0];
							float y = (float)scalingVector[1];
							float z = (float)scalingVector[2];
							float frameSeconds = (float)frameTime.GetSecondDouble(); // If needed, get the time of the scaling keyframe, in seconds

							newAnimation->AddKeyFrame(layerIndex, boneIter.second, new KeyFrameScaling(glm::vec3(x, y, z), frameSeconds));
						}
					}
				}

				{
					if (translationCurve == nullptr)
					{
						FbxDouble3 translationVector = fbxNode->LclTranslation.Get();
						float x = (float)translationVector[0];
						float y = (float)translationVector[1];
						float z = (float)translationVector[2];
						newAnimation->AddKeyFrame(layerIndex, boneIter.second, new KeyFrameTranslation(glm::vec3(x, y, z), 0));
					}
					else
					{
						int numKeys = translationCurve->KeyGetCount();
						for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
						{
							FbxTime frameTime = translationCurve->KeyGetTime(keyIndex);
							FbxDouble3 translationVector = fbxNode->EvaluateLocalTranslation(frameTime);
							float x = (float)translationVector[0];
							float y = (float)translationVector[1];
							float z = (float)translationVector[2];
							float frameSeconds = (float)frameTime.GetSecondDouble(); // If needed, get the time of the scaling keyframe, in seconds

							newAnimation->AddKeyFrame(layerIndex, boneIter.second, new KeyFrameTranslation(glm::vec3(x, y, z), frameSeconds));
						}
					}
				}

				{
					if (rotationCurve == nullptr)
					{
						FbxVector4 rotationCurve = fbxNode->EvaluateLocalRotation(0);
						float x = (float)rotationCurve[0];
						float y = (float)rotationCurve[1];
						float z = (float)rotationCurve[2];
						float w = (float)rotationCurve[3];

						newAnimation->AddKeyFrame(layerIndex, boneIter.second, new KeyFrameRotation(glm::quat(x, y, z, w), 0));
					}
					else
					{
						int numKeys = rotationCurve->KeyGetCount();
						for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
						{
							FbxTime frameTime = rotationCurve->KeyGetTime(keyIndex);
							FbxVector4 rotationCurve = fbxNode->EvaluateLocalRotation(frameTime);
							float x = (float)rotationCurve[0];
							float y = (float)rotationCurve[1];
							float z = (float)rotationCurve[2];
							float w = (float)rotationCurve[3];
							float frameSeconds = (float)frameTime.GetSecondDouble(); // If needed, get the time of the scaling keyframe, in seconds

							newAnimation->AddKeyFrame(layerIndex, boneIter.second, new KeyFrameRotation(glm::quat(x, y, z, w), frameSeconds));
						}
					}
				}
			}
		}

		animations.push_back(newAnimation);
	}

	return animations;
}