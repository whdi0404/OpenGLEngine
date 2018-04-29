#include "stdafx.h"
#include "ResourceManager.h"
#include "Object.h"
#include "VertexBuffer.h"
#include "PhysXUtil.h"
#include "Texture.h"

ResourceManager::ResourceManager()
{
}


ResourceManager::~ResourceManager()
{
}

void ResourceManager::SearchResources()
{
	FileCheck([](std::string extension, std::string filePath)
	{
		if (extension == ".bmp" ||
			extension == ".jpg" ||
			extension == ".png" ||
			extension == ".tga" ||
			extension == ".dds")
		{
			Texture2D* texture = new Texture2D(filePath);
			return new ResourceNode(texture);
		}
		return (ResourceNode*)nullptr;
	});

	FileCheck([](std::string extension, std::string filePath)
	{
		if (extension == ".fbx")
		{
			std::vector<Object*> resources = FBXHelper::GetResourcesFromFile(filePath);
			return new ResourceNode(nullptr, resources);
		}
		return (ResourceNode*)nullptr;
	});
}

void ResourceManager::FileCheck(std::function<ResourceNode* (std::string, std::string)> createResource)
{
	directory_entry resourceRoot = directory_entry("./Resources");
	std::queue<directory_entry> entries;
	entries.push(resourceRoot);
	
	std::vector<directory_entry> all;

	while (entries.empty() == false)
	{
		directory_entry entry = entries.front();
		entries.pop();
		all.push_back(entry);		
		
		for (auto e : directory_iterator(entry))
		{
			file_type fileType = e.status().type();
			if (fileType == file_type::directory)
				entries.push(e);
			else if (e.path().has_extension() == true)
			{
				std::string extension = GetExtension(e.path().string());

				std::locale loc;
				for (int i = 0; i < extension.size(); ++i)
					extension[i] = std::tolower(extension[i], loc);

				std::string key = GetPathToKey(e.path().string());

				ResourceNode* resourceNode = createResource(extension, e.path().string());
				if(resourceNode != nullptr)
					AddResource(key, resourceNode);
			}
		}
	}
}

//void ResourceManager::AddPxTriangleMeshGeometryResource(std::string key, Mesh* renderMesh, PxVec3 sc, PxQuat rot)
//{
//	PxGeometry* geometry = PhysXUtil::MeshToPxGeometry(renderMesh, sc, rot);
//	AddResource(key, geometry);
//}
//
//void ResourceManager::AddPxConvexMeshGeometryResource(std::string key, Mesh* renderMesh, PxVec3 sc, PxQuat rot)
//{
//	PxGeometry* geometry = PhysXUtil::MeshToPxConvexGeometry(renderMesh, sc, rot);
//	AddResource(key, geometry);
//}
//
//void ResourceManager::AddPxSphereGeometryResource(std::string key, float radius)
//{
//	PxGeometry* geometry = new PxSphereGeometry(radius);
//	AddResource(key, geometry);
//}
//
//void ResourceManager::AddPxCapsuleGeometryResource(std::string key, float radius, float height)
//{
//	PxGeometry* geometry = new PxCapsuleGeometry(radius, height);
//	AddResource(key, geometry);
//}
//
//void ResourceManager::AddPxBoxGeometryResource(std::string key, glm::vec3 boxHalfSize)
//{
//	PxGeometry* geometry = new PxBoxGeometry(boxHalfSize.x, boxHalfSize.y, boxHalfSize.z);
//	AddResource(key, geometry);
//}