#include "stdafx.h"
#include "FBXHelper.h"
#include "Mesh.h"
#include "SkinnedMesh.h"
#include "VertexBuffer.h"
#include "MeshVertexAttribute.h"
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
			FbxNode* pFbxRootNode = pFbxScene->GetRootNode();
			stack.push(pFbxRootNode);
			while (stack.empty() == false)
			{
				FbxNode* node = stack.top();
				stack.pop();

				for (int i = 0; i < node->GetChildCount(); ++i)
				{
					FbxNode* childNode = node->GetChild(i);

					LoadNode(returnObject, pFbxScene, childNode, modelTransform);

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

void FBXHelper::LoadNode(std::vector<Object*>& refObject, FbxScene* fbxScene, FbxNode* fbxNode, glm::mat4x4 modelTransform)
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

			Mesh* mesh = LoadMeshData(fbxScene, (FbxMesh*)nodeAttributeFbx, modelTransform);
			if (mesh == nullptr)
				continue;

			//if (dynamic_cast<SkinnedMesh*>(mesh) != nullptr)
			//	LoadNodeKeyframeAnimation(fbxScene, fbxNode);

			refObject.push_back(mesh);
		}
		break;
		}
	}
}

Mesh* FBXHelper::LoadMeshData(FbxScene* fbxScene, FbxMesh *fbxMesh, glm::mat4x4 modelTransform)
{
	FbxPose* lPose = fbxScene->GetPose(0);

	VertexBuffer* vertexBuffer;
	std::vector<int> indices;

	std::vector<vec3> position;
	std::vector<vec2> uvs;

	FbxVector4* pVertices = fbxMesh->GetControlPoints();

	FbxLayerElementArrayTemplate<FbxVector2>* pTexUvs;
	fbxMesh->GetTextureUV(&pTexUvs);
	if (pTexUvs == nullptr)
		return nullptr;
	FbxLayerElementUV* layerUVs = fbxMesh->GetLayer(0)->GetUVs();

	FbxLayerElement::EReferenceMode referenceMode = layerUVs->GetReferenceMode();
	FbxLayerElement::EMappingMode mappingMode = layerUVs->GetMappingMode();

	std::cout << mappingMode << std::endl;

	int vertexCount = fbxMesh->GetControlPointsCount();
	//pOutPosition->resize(vertexCount);
	bool uvSet = false;
	uvs.resize(vertexCount);
	for (int j = 0; j < vertexCount; ++j)
	{
		vec3 vertex;
		vertex.x = (float)pVertices[j].mData[0];
		vertex.y = (float)pVertices[j].mData[1];
		vertex.z = (float)pVertices[j].mData[2];

		vec4 rotVec = /*modelTransform * */vec4(vertex.x, vertex.y, vertex.z, 1);
		position.push_back(vec3(rotVec.x, rotVec.y, rotVec.z));
		//(*pOutPosition)[j] = vertex;

		if (FbxLayerElement::eByControlPoint == mappingMode)
		{
			int uvIndex = 0;
			if (FbxLayerElement::eDirect == referenceMode)
				uvIndex = j;
			else if (FbxLayerElement::eIndexToDirect == referenceMode)
				uvIndex = layerUVs->GetIndexArray().GetAt(j);
			vec2 uv;
			uv.x = (*pTexUvs)[uvIndex].mData[0];
			uv.y = (*pTexUvs)[uvIndex].mData[1];
			uvs[uvIndex] = uv;
			uvSet = true;
		}
	}

	int polygonCount = fbxMesh->GetPolygonCount();
	//pOutindices->resize(polygonCount * 3);
	for (int j = 0; j < polygonCount; j++)
	{
		int iNumVertices = fbxMesh->GetPolygonSize(j);

		if (iNumVertices == 3)
		{
			indices.push_back(fbxMesh->GetPolygonVertex(j, 0));
			indices.push_back(fbxMesh->GetPolygonVertex(j, 1));
			indices.push_back(fbxMesh->GetPolygonVertex(j, 2));
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

		// 씨이이이발! Stiky Memo 참고
		
		if (FbxLayerElement::eByPolygonVertex == mappingMode)
		{
			for (int k = 0; k < iNumVertices; k++)
			{
				int iControlPointIndex = fbxMesh->GetPolygonVertex(j, k);

				int uvIndex = 0;
				int id = fbxMesh->GetTextureUVIndex(j, k);
				if (FbxLayerElement::eDirect == referenceMode ||
					FbxLayerElement::eIndexToDirect == referenceMode)
					uvIndex = id;
				else
					continue;

				vec2 uv;
				uv.x = (*pTexUvs)[uvIndex].mData[0];
				uv.y = (*pTexUvs)[uvIndex].mData[1];
				uvs[iControlPointIndex] = uv;
			}
		}
	}

	int numDeformers = fbxMesh->GetDeformerCount();
	FbxSkin* skin = (FbxSkin*)fbxMesh->GetDeformer(0, FbxDeformer::eSkin);

	std::vector<mat4x4> temps;
	if (skin != nullptr && skin->GetClusterCount() != 0)
	{
		FbxAMatrix* lClusterDeformation = new FbxAMatrix[vertexCount];
		memset(lClusterDeformation, 0, vertexCount * sizeof(FbxAMatrix));

		double* lClusterWeight = new double[vertexCount];
		memset(lClusterWeight, 0, vertexCount * sizeof(double));

		FbxCluster::ELinkMode lClusterMode = skin->GetCluster(0)->GetLinkMode();
		FbxSkin::EType skinningType = skin->GetSkinningType();

		std::cout << skinningType << std::endl;

		SkinnedMesh* newMesh = new SkinnedMesh();

		std::vector<vec4> boneWeights;
		std::vector<vec4> boneIndices;
		boneWeights.resize(position.size(), vec4(-1, -1, -1, -1));
		boneIndices.resize(position.size(), vec4(-1, -1, -1, -1));

		//TODO: FBX씬에있는 모든 하이어라키를 가져와서 넣자.
		//쉐이더에 들어갈 행렬은 bone들만.
		//에니메이션하는 행렬들은 bone들 뿐일것임.

		int boneCount = skin->GetClusterCount();

		for (int lClusterIndex = 0; lClusterIndex < boneCount; ++lClusterIndex)
		{
			FbxCluster* lCluster = skin->GetCluster(lClusterIndex);
			FbxNode* node = lCluster->GetLink();
			if (node == nullptr)
				continue;

			FbxAMatrix deformationMatrix = ComputeClusterDeformation(fbxMesh, lCluster);
			FbxAMatrix animationMatrix = GetAnimationMatrix(lCluster, 0, lPose);
			temps.push_back(GetGLMMatrixFromFBXMatrix(deformationMatrix));

			Transform* boneTransform = new Transform();
			std::string name = node->GetName();
			boneTransform->SetName(name);
			//deformationMatrix를 별도로 보관하자. animationCluster에서 animationMatrix만 얻어서 deformationMatrix와 곱해서 계산하도록.
			boneTransform->SetLocalMatrix(GetGLMMatrixFromFBXMatrix(animationMatrix));
			newMesh->AddBone(boneTransform, GetGLMMatrixFromFBXMatrix(deformationMatrix));

			int lVertexIndexCount = lCluster->GetControlPointIndicesCount();
			for (int k = 0; k < lVertexIndexCount; ++k)
			{
				int lIndex = lCluster->GetControlPointIndices()[k];

				if (lIndex >= vertexCount)
					continue;

				double lWeight = lCluster->GetControlPointWeights()[k];

				if (lWeight == 0.0)
				{
					continue;
				}

				FbxAMatrix lInfluence = deformationMatrix;
				MatrixScale(lInfluence, lWeight);

				MatrixAdd(lClusterDeformation[lIndex], lInfluence);

				lClusterWeight[lIndex] += lWeight;
			}
		}

		for (int boneIndex = 0; boneIndex < boneCount; boneIndex++)
		{
			FbxCluster* cluster = skin->GetCluster(boneIndex);
			FbxNode* bone = cluster->GetLink(); // Get a reference to the bone's node
												// Get the bind pose

			FbxNode* parentBone = bone->GetParent();
			if (parentBone != nullptr)
			{
				Transform* parentTransform = newMesh->GetBone(parentBone->GetName());
				if (parentTransform != nullptr)
					newMesh->GetBone(bone->GetName())->SetParent(parentTransform);
				else
					newMesh->SetRoot(newMesh->GetBone(bone->GetName()));
			}

			//Blending Infos
			int *boneVertexIndices = cluster->GetControlPointIndices();
			double *boneVertexWeights = cluster->GetControlPointWeights();
			// Iterate through all the vertices, which are affected by the bone
			int numBoneVertexIndices = cluster->GetControlPointIndicesCount();

			for (int i = 0; i < numBoneVertexIndices; i++)
			{
				float weight = (float)boneVertexWeights[i];

				int boneVertexIndex = boneVertexIndices[i];
				if (boneIndices[boneVertexIndex].x == -1)
				{
					boneIndices[boneVertexIndex].x = boneIndex;
					boneWeights[boneVertexIndex].x = weight;
				}
				else if (boneIndices[boneVertexIndex].y == -1)
				{
					boneIndices[boneVertexIndex].y = boneIndex;
					boneWeights[boneVertexIndex].y = weight;
				}
				else if (boneIndices[boneVertexIndex].z == -1)
				{
					boneIndices[boneVertexIndex].z = boneIndex;
					boneWeights[boneVertexIndex].z = weight;
				}
				else if (boneIndices[boneVertexIndex].w == -1)
				{
					boneIndices[boneVertexIndex].w = boneIndex;
					boneWeights[boneVertexIndex].w = weight;
				}
				else
					continue;
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
		std::for_each(boneIndices.begin(), boneIndices.end(), [](vec4& v)
		{
			if (v.x == -1) v.x = 0;
			if (v.y == -1) v.y = 0;
			if (v.z == -1) v.z = 0;
			if (v.w == -1) v.w = 0;
		});

		vertexBuffer = new VertexBuffer(SkinningVertexAttribute);
		vertexBuffer->SetVertexCount(position.size());
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

void FBXHelper::LoadNodeKeyframeAnimation(FbxScene* fbxScene, FbxNode* fbxNode)
{
	bool isAnimated = false;

	int srcObjectCount = fbxScene->GetSrcObjectCount<FbxAnimStack>();
	for (int i = 0; i < srcObjectCount; ++i)
	{
		FbxAnimStack* lAnimStack = fbxScene->GetSrcObject<FbxAnimStack>(i);

		FbxString stackName = "Animation Stack Name: ";
		stackName += lAnimStack->GetName();
		std::string sStackName = stackName;

		int numLayers = lAnimStack->GetMemberCount<FbxAnimLayer>();
		for (int j = 0; j < numLayers; ++j)
		{
			FbxAnimLayer* lAnimLayer = lAnimStack->GetMember<FbxAnimLayer>(j);

			//FbxString layerName = "  Animation Layer Name: ";
			//layerName += lAnimLayer->GetName();
			//std::string sLayerName = layerName;

			//std::queue<BoneInfo> queueNodes;
			//queueNodes.push(BoneInfo(fbxScene->GetRootNode()));

			//while (queueNodes.empty() == false)
			//{
			//	BoneInfo node = queueNodes.front();
			//	queueNodes.pop();

			//	int childNode = node.fbxNode->GetChildCount();
			//	for (int k = 0; k < childNode; ++k)
			//	{
			//		FbxNode* childNode = node.fbxNode->GetChild(k);
			//		queueNodes.push(BoneInfo(childNode, node.boneTransform));
			//	}
			//	FbxAnimCurve *translateCurve = node.fbxNode->LclTranslation.GetCurve(lAnimLayer);
			//	if (translateCurve == nullptr)
			//		continue;
			//	FbxAnimCurve *rotationCurve = node.fbxNode->LclRotation.GetCurve(lAnimLayer);
			//	FbxAnimCurve *scalingCurve = node.fbxNode->LclScaling.GetCurve(lAnimLayer);

			//	int numKeys = translateCurve->KeyGetCount();
			//	for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
			//	{
			//		//Todo: 각 행렬마다 프레임별로 저장해둬야함.
			//		FbxTime frameTime = translateCurve->KeyGetTime(keyIndex);
			//		FbxAMatrix fbxMatrix = node.fbxNode->EvaluateLocalTransform(frameTime);
			//		glm::mat4x4 mat;
			//		auto double44 = fbxMatrix.Double44();
			//		for (int col = 0; col < 4; ++col)
			//			for(int row = 0; row < 4 ; ++row)
			//				mat[col][row] = double44[col][row];
			//		node.boneTransform->SetLocalMatrix(mat);
			//		//FbxVector4 rotation = node.fbxNode->EvaluateLocalRotation(frameTime);
			//		//glm::quat quaternion = glm::quat((float)rotation[0], (float)rotation[1], (float)rotation[2], (float)rotation[3]);
			//		//
			//		//FbxVector4 position = node.fbxNode->EvaluateLocalTransform(lAnimLayer);
			//		//glm::vec3 translate = glm::vec3((float)position[0], (float)position[1], (float)position[2]);
			//		//node.boneTransform->SetRotateLocal(quaternion);
			//		//node.boneTransform->SetLocalPosition(translate);
			//		//
			//		//float frameSeconds = (float)frameTime.GetSecondDouble(); // If needed, get the time of the scaling keyframe, in seconds
			//	}
			//	if (scalingCurve != nullptr)
			//	{
			//
			//	}
			//	else
			//	{
			//		// If this animation layer has no scaling curve, then use the default one, if needed
			//		FbxDouble3 scalingVector = fbxNode->LclScaling.Get();
			//		float x = (float)scalingVector[0];
			//		float y = (float)scalingVector[1];
			//		float z = (float)scalingVector[2];
			//	}
			//}
		}
	}

	// Iterate all animations (for example, walking, running, falling and etc.)
	//int numAnimations = fbxScene->GetSrcObjectCount(FbxAnimStack::ClassId);
	//for (int animationIndex = 0; animationIndex < numAnimations; animationIndex++)
	//{
	//	FbxAnimStack *animStack = (FbxAnimStack*)fbxScene->GetSrcObject(FbxAnimStack::ClassId, animationIndex);
	//	FbxAnimEvaluator *animEvaluator = fbxScene->GetAnimationEvaluator();
	//	animStack->GetName(); // Get the name of the animation if needed

	//						  // Iterate all the transformation layers of the animation. You can have several layers, for example one for translation, one for rotation, one for scaling and each can have keys at different frame numbers.
	//	int numLayers = animStack->GetMemberCount();
	//	for (int layerIndex = 0; layerIndex < numLayers; layerIndex++)
	//	{
	//		FbxAnimLayer *animLayer = (FbxAnimLayer*)animStack->GetMember(layerIndex);
	//		animLayer->GetName(); // Get the layer's name if needed

	//		FbxAnimCurve *translationCurve = fbxNode->LclTranslation.GetCurve(animLayer);
	//		FbxAnimCurve *rotationCurve = fbxNode->LclRotation.GetCurve(animLayer);
	//		FbxAnimCurve *scalingCurve = fbxNode->LclScaling.GetCurve(animLayer);

	//		if (scalingCurve != 0)
	//		{
	//			int numKeys = scalingCurve->KeyGetCount();
	//			for (int keyIndex = 0; keyIndex < numKeys; keyIndex++)
	//			{
	//				FbxTime frameTime = scalingCurve->KeyGetTime(keyIndex);
	//				FbxDouble3 scalingVector = fbxNode->EvaluateLocalScaling(frameTime);
	//				float x = (float)scalingVector[0];
	//				float y = (float)scalingVector[1];
	//				float z = (float)scalingVector[2];

	//				float frameSeconds = (float)frameTime.GetSecondDouble(); // If needed, get the time of the scaling keyframe, in seconds
	//			}
	//		}
	//		else
	//		{
	//			// If this animation layer has no scaling curve, then use the default one, if needed
	//			FbxDouble3 scalingVector = fbxNode->LclScaling.Get();
	//			float x = (float)scalingVector[0];
	//			float y = (float)scalingVector[1];
	//			float z = (float)scalingVector[2];
	//		}

	//		// Analogically, process rotationa and translation
	//	}
	//}
}

//animationMatrix얻는 함수로 분리하자.
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
		// There is no pose entry for that node, get the current global position instead.

		// Ideally this would use parent global position and local position to compute the global position.
		// Unfortunately the equation
		//    lGlobalPosition = pParentGlobalPosition * lLocalPosition
		// does not hold when inheritance type is other than "Parent" (RSrs).
		// To compute the parent rotation and scaling is tricky in the RrSs and Rrs cases.
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