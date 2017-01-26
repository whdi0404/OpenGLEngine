#pragma once

class Mesh;
class SkinnedMesh;
class Object;
class VertexBuffer;
class Transform;
class FBXHelper
{
public:
	static std::vector<Object*> GetResourcesFromFile(std::string filePath);

private:
	static void LoadNode(std::vector<Object*>& refObject, FbxScene* fbxScene, FbxNode* fbxNode);

	static Mesh* LoadMeshData(FbxScene* fbxScene, FbxMesh *fbxMesh);
	static void LoadNodeKeyframeAnimation(FbxScene* fbxScene, FbxNode* anim);

	static Transform* GetBones(FbxNode* node);
};