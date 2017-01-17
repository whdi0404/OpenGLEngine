#pragma once

class Mesh;
class SkinnedMesh;
class Object;
class VertexBuffer;
class FBXHelper
{
public:
	static std::vector<Object*> GetResourcesFromFile(std::string filePath);

private:
	static void LoadNode(std::vector<Object*>& refObject, FbxScene* fbxScene, FbxNode* fbxNode);

	static bool LoadMeshData(FbxMesh *fbxMesh, VertexBuffer*& outVertexBuffer, std::vector<int>& outIndices);
	static void LoadNodeKeyframeAnimation(FbxScene* fbxScene, FbxNode* anim);

	static void DrawHierarchy(FbxNode* node, int depth = 0);
};