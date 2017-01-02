#include "stdafx.h"
#include "Resources.h"

/*
Resources::Resources()
{
}


Resources::~Resources()
{
}

bool Resources::Register(std::string key, Object * resource)
{
	auto iter = v_Resources.find(key);
	
	if (iter == v_Resources.end())
	{
		v_Resources.insert(std::unordered_map<std::string, Object*>::value_type(key, resource));
		return true;
	}
	else
		return false;
}

void Resources::UnRegister(Object* resource)
{
	for (auto iter = v_Resources.begin(); iter != v_Resources.end(); ++iter)
	{
		if (iter->second == resource)
		{
			v_Resources.erase(iter);
			break;
		}
	}
	
}

void Resources::UnRegister(std::string key)
{
	auto iter = v_Resources.find(key);
	if (iter != v_Resources.end())
	{
		v_Resources.erase(iter);
	}
}
*/