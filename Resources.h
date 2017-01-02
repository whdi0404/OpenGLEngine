#pragma once

/*
class Resources
{
private:
	Resources();
	~Resources();

public:
	template<typename T>
	static T* Load(std::string key);

	template<typename T>
	static std::vector<T*> LoadAll();
	
	static bool Register(std::string key, Object* resource);
	static void UnRegister(Object* resource);
	static void UnRegister(std::string key);

	static std::unordered_map<std::string, Object*> v_Resources;
};

template<typename T>
inline T * Resources::Load(std::string key)
{
	auto iter = v_Resources.find(key);
	if (iter == v_Resources.end())
		return nullptr;
	else
		return iter->second;
}

template<typename T>
inline std::vector<T*> Resources::LoadAll()
{
	std::vector<T*> returnValue;
	for(auto iter = v_Resources.begin();iter != v_Resources.end();++iter)
	{
		if (dynamic_cast<T*>(iter->second) != nullptr)
		{		
			returnValue.push_back((T*)iter->second);
		}
	}

	return returnValue;
}
*/