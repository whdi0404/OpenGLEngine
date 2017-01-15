#include "stdafx.h"
#include "FBXHelper.h"
#include "Mesh.h"
#include "SkinnedMesh.h"

std::stack<FbxNode*> stack;
std::vector<Object*> FBXHelper::GetResourcesFromFile(std::string filePath)
{
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
			FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
			stack.push(pFbxRootNode);
			while (stack.empty())
			{
				FbxNode* node = stack.top();
				stack.pop();

				for (int i = 0; i < node->GetChildCount(); ++i)
				{
					FbxNode* childNode = node->GetChild(i);

					Object* obj = LoadNode(pFbxScene, childNode);
					if (obj != nullptr)
						returnObject.push_back(obj);

					if (childNode->GetChildCount() != 0)
						stack.push(childNode);
				}
			}
			pFbxRootNode->Destroy();
		}
	}

	pImporter->Destroy();
	pFbxScene->Destroy();

	return returnObject;
}

Object * FBXHelper::LoadNode(FbxScene* fbxScene, FbxNode* fbxNode)
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
			LoadMesh((FbxMesh*)nodeAttributeFbx);
			LoadSkinnedMesh((FbxMesh*)nodeAttributeFbx);
			LoadNodeKeyframeAnimation(fbxScene, fbxNode);
			break;
		}
		}
	}
}

Mesh * FBXHelper::LoadMesh(FbxMesh * fbxMesh)
{
	Mesh* mesh = new Mesh();
	std::vector<vec3> position;
	std::vector<int> indices;
	std::vector<vec2> uvs;

	FbxVector4* pVertices = fbxMesh->GetControlPoints();
	FbxLayerElementArrayTemplate<FbxVector2>* pTexUvs;
	fbxMesh->GetTextureUV(&pTexUvs);
	if (pTexUvs == nullptr)
		return nullptr;
	int vertexCount = fbxMesh->GetControlPointsCount();
	int count = pTexUvs->GetCount();
	//pOutPosition->resize(vertexCount);
	for (int j = 0; j < vertexCount; ++j)
	{
		vec3 vertex;
		vertex.x = (float)pVertices[j].mData[0];
		vertex.y = (float)pVertices[j].mData[1];
		vertex.z = (float)pVertices[j].mData[2];
		position.push_back(vertex);
		//(*pOutPosition)[j] = vertex;

		vec2 uv;
		uv.x = (*pTexUvs)[j].mData[0];
		uv.y = (*pTexUvs)[j].mData[1];
		uvs.push_back(uv);
	}

	int polygonCount = fbxMesh->GetPolygonCount();

	//pOutindices->resize(polygonCount * 3);
	for (int j = 0; j < polygonCount; j++)
	{
		int iNumVertices = fbxMesh->GetPolygonSize(j);
		if (iNumVertices == 3)
		{
			for (int k = 0; k < 3; k++)
			{
				int iControlPointIndex = fbxMesh->GetPolygonVertex(j, k);
				indices.push_back(iControlPointIndex);
				//(*pOutindices)[(j * 3) + k] = iControlPointIndex;
			}
		}
		if (iNumVertices == 4)
		{
			indices.push_back(fbxMesh->GetPolygonVertex(j, 0));
			indices.push_back(fbxMesh->GetPolygonVertex(j, 1));
			indices.push_back(fbxMesh->GetPolygonVertex(j, 2));

			indices.push_back(fbxMesh->GetPolygonVertex(j, 0));
			indices.push_back(fbxMesh->GetPolygonVertex(j, 2));
			indices.push_back(fbxMesh->GetPolygonVertex(j, 3));
		}
	}

	std::vector<DefaultVertex> vertices;
	int size = position.size();
	for (int i = 0; i < size; ++i)
	{
		DefaultVertex vertex(position[i], uvs[i]);
		vertices.push_back(vertex);
	}

	mesh->SetVertexBuffer(vertices, indices);
	return mesh;
}

SkinnedMesh * FBXHelper::LoadSkinnedMesh(FbxMesh * fbxMesh)
{//Bones
	int numDeformers = fbxMesh->GetDeformerCount();
	FbxSkin* skin = (FbxSkin*)fbxMesh->GetDeformer(0, FbxDeformer::eSkin);
	if (skin != 0)
	{
		int boneCount = skin->GetClusterCount();
		for (int boneIndex = 0; boneIndex < boneCount; boneIndex++)
		{
			FbxCluster* cluster = skin->GetCluster(boneIndex);
			FbxNode* bone = cluster->GetLink(); // Get a reference to the bone's node
												// Get the bind pose
			FbxAMatrix bindPoseMatrix;
			cluster->GetTransformLinkMatrix(bindPoseMatrix);

			int *boneVertexIndices = cluster->GetControlPointIndices();
			double *boneVertexWeights = cluster->GetControlPointWeights();

			// Iterate through all the vertices, which are affected by the bone
			int numBoneVertexIndices = cluster->GetControlPointIndicesCount();
			for (int i = 0; i < numBoneVertexIndices; i++)
			{
				int boneVertexIndex = boneVertexIndices[i];
				float boneWeight = (float)boneVertexWeights[boneVertexIndex];
			}
		}
	}
}

void FBXHelper::LoadNodeKeyframeAnimation(FbxScene* fbxScene, FbxNode* fbxNode)
{
	bool isAnimated = false;

	// Iterate all animations (for example, walking, running, falling and etc.)
	int numAnimations = fbxScene->GetSrcObjectCount(FbxAnimStack::ClassId);
	for (int animationIndex = 0; animationIndex < numAnimations; animationIndex++)
	{
		FbxAnimStack *animStack = (FbxAnimStack*)fbxScene->GetSrcObject(FbxAnimStack::ClassId, animationIndex);
		FbxAnimEvaluator *animEvaluator = fbxScene->GetAnimationEvaluator();
		animStack->GetName(); // Get the name of the animation if needed

							  // Iterate all the transformation layers of the animation. You can have several layers, for example one for translation, one for rotation, one for scaling and each can have keys at different frame numbers.
		int numLayers = animStack->GetMemberCount();
		for (int layerIndex = 0; layerIndex < numLayers; layerIndex++)
		{
			FbxAnimLayer *animLayer = (FbxAnimLayer*)animStack->GetMember(layerIndex);
			animLayer->GetName(); // Get the layer's name if needed

			FbxAnimCurve *translationCurve = fbxNode->LclTranslation.GetCurve(animLayer);
			FbxAnimCurve *rotationCurve = fbxNode->LclRotation.GetCurve(animLayer);
			FbxAnimCurve *scalingCurve = fbxNode->LclScaling.GetCurve(animLayer);

			if (scalingCurve != 0)
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
				}
			}
			else
			{
				// If this animation layer has no scaling curve, then use the default one, if needed
				FbxDouble3 scalingVector = fbxNode->LclScaling.Get();
				float x = (float)scalingVector[0];
				float y = (float)scalingVector[1];
				float z = (float)scalingVector[2];
			}

			// Analogically, process rotationa and translation
		}
	}
}