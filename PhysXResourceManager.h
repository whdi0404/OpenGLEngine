#pragma once

class Mesh;
class PhysXResourceManager
{
	//[typeId][Name]
private:
	std::unordered_map<std::string, std::unordered_map<std::string, void*>> resources;
public:
	PhysXResourceManager();
	~PhysXResourceManager();

	template<class T>
	T* GetResource(std::string key)
	{
		auto& types = resources.find(typeid(T*).name());
		if (types != resources.end())
		{
			auto& resource = types->second.find(key);
			if (resource != types->second.end())
				return (T*)resource->second;
		}
		return nullptr;
	}

	template<class T>
	void AddResource(std::string key, T* value)
	{
		std::string typeName = typeid(T*).name();
		auto& types = resources.find(typeName);
		if (types != resources.end())
		{
			types->second.insert(std::make_pair(key, value));
		}
		else
		{
			auto rec = std::unordered_map<std::string, void*>();
			rec.insert(std::make_pair(key, value));
			resources.insert(std::make_pair(typeName, rec));
		}
	}

	void AddGeometryResource(std::string key, Mesh* renderMesh, PxVec3 scale = PxVec3(1, 1, 1), PxQuat rot = PxQuat(PxIdentity));
	void AddSphereGeomResource(std::string key, float radius);
};