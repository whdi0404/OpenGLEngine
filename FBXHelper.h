#pragma once

class Mesh;
class SkinnedMesh;
class Object;
class VertexBuffer;
class Transform;
class FBXHelper
{
public:
	static std::vector<Object*> GetResourcesFromFile(std::string filePath, glm::mat4x4 modelTransform);

private:
	static void LoadNode(std::vector<Object*>& refObject, FbxScene* fbxScene, FbxNode* fbxNode, glm::mat4x4 modelTransform);

	static Mesh* LoadMeshData(FbxScene* fbxScene, FbxMesh *fbxMesh, glm::mat4x4 modelTransform);
	static void LoadNodeKeyframeAnimation(FbxScene* fbxScene, FbxNode* anim);
};