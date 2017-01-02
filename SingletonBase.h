#pragma once

template <typename Class>
class Singleton
{
private:
	static Class* instance;
public:
	static Class& GetInstance()
	{
		if (instance == nullptr)
		{
			instance = new Class;
		}
		return *instance;
	}
	static Class* GetInstancePtr()
	{
		if (instance == nullptr)
		{
			instance = new Class;
		}
		return instance;
	}

	static void ReleaseInstance()
	{
		if (instance != nullptr)
		{
			delete instance;
			instance = nullptr;
		}
	}
};

template<typename Class> 
 Class* Singleton<Class>::instance = 0;