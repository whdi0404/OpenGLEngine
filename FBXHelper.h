#pragma once

class Mesh;
class SkinnedMesh;
class Object;
namespace FBXHelper
{
	std::vector<Object*> GetResourcesFromFile(std::string filePath);

	Object* LoadNode(FbxScene* scene, FbxNode* fbxNode);

	Mesh* LoadMesh(FbxMesh *fbxMesh);
	SkinnedMesh* LoadSkinnedMesh(FbxMesh* fbxMesh);
	void LoadNodeKeyframeAnimation(FbxScene* scene, FbxNode* anim);
}