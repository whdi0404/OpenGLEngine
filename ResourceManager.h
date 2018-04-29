#pragma once
#include <vector>

class Object;
class ResourceNode
{
private:
	Object* resource;
	std::vector<Object*> children;

public:
	ResourceNode(Object* resource)
	{
		this->resource = resource;
	}

	ResourceNode(Object* resource, std::vector<Object*> children)
	{
		this->resource = resource;
		this->children = std::vector<Object*>(children);
	}

	void AddChildResource(Object* resourceNode)
	{
		children.push_back(resourceNode);
	}

	template<class T>
	T* GetResource()
	{
		return dynamic_cast<T*>(resource);
	}

	template<class T>
	std::vector<T*> GetResourceAll()
	{
		std::vector<T*> resources;
		for (auto& child : children)
		{
			T* res = dynamic_cast<T*>(child);
			if (res != nullptr)
				resources.push_back(res);
		}
		return resources;
	}
};

class Object;
class ResourceManager : public Singleton<ResourceManager>
{
private:
	std::unordered_map<std::string, ResourceNode*> resources;

public:
	ResourceManager();
	~ResourceManager();

private:
	void FileCheck(std::function<ResourceNode* (std::string, std::string)> createResource);


public:
	void SearchResources();

	void AddResource(std::string key, ResourceNode* resoureceNode)
	{
		resources.insert(std::make_pair(key, resoureceNode));
	}

	template<class T>
	T* GetResource(std::string key)
	{
		std::replace_if(key.begin(), key.end(), [](char c) { return c == '/'; }, '\\');

		key = GetPathToKey(key);
		auto& resource = resources.find(key);
		if (resource != resources.end())
		{
			return resource->second->GetResource<T>();
		}
		return nullptr;
	}

	template<class T>
	std::vector<T*> GetResourceAll(std::string key)
	{
		std::replace_if(key.begin(), key.end(), [](char c) { return c == '/'; }, '\\');

		key = GetPathToKey(key);
		auto& resource = resources.find(key);
		if (resource != resources.end())
		{
			return resource->second->GetResourceAll<T>();
		}
		return std::vector<T*>();
	}

	std::string GetExtension(std::string path)
	{
		std::experimental::filesystem::path e = std::experimental::filesystem::path(path);
		return e.extension().string();
	}

	std::string GetPathToKey(std::string path)
	{
		std::experimental::filesystem::path e = std::experimental::filesystem::path(path);
		std::string key = e.string();

		if (e.has_extension() == true)
			key = e.string().erase(e.string().size() - e.extension().string().size(), e.extension().string().size());
		if (key.find(".\\Resources\\") == 0)
			key = key.erase(0, std::string(".\\Resources\\").size());

		return key;
	}

	/*void AddPxTriangleMeshGeometryResource(std::string key, Mesh* renderMesh, PxVec3 scale = PxVec3(1, 1, 1), PxQuat rot = PxQuat(PxIdentity));
	void AddPxConvexMeshGeometryResource(std::string key, Mesh* renderMesh, PxVec3 scale = PxVec3(1, 1, 1), PxQuat rot = PxQuat(PxIdentity));
	void AddPxSphereGeometryResource(std::string key, float radius);
	void AddPxCapsuleGeometryResource(std::string key, float radius, float height);
	void AddPxBoxGeometryResource(std::string key, glm::vec3 boxSize);*/
};