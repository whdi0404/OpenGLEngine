#pragma once

class Mesh;
class SkinnedMesh;
class Object;
class FBXHelper
{
public:
	static std::vector<Object*> GetResourcesFromFile(std::string filePath);

private:
	static void LoadNode(std::vector<Object*>& refObject, FbxScene* fbxScene, FbxNode* fbxNode);

	static Mesh* LoadMesh(FbxMesh *fbxMesh);
	static SkinnedMesh* LoadSkinnedMesh(FbxMesh* fbxMesh);
	static void LoadNodeKeyframeAnimation(FbxScene* fbxScene, FbxNode* anim);
};