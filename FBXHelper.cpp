#include "stdafx.h"
#include "FBXHelper.h"

std::vector<Object*> FBXHelper::GetResourcesFromFile(std::string filePath, FBXCreateAction createAction)
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

			for (int i = 0; i < pFbxRootNode->GetChildCount(); ++i)
			{
				Object* obj = createAction(pFbxRootNode->GetChild(i));
				if(obj != nullptr)
					returnObject.push_back(obj);
			}
			pFbxRootNode->Destroy();
		}
	}
	pImporter->Destroy();
	pFbxScene->Destroy();

	return returnObject;
}
