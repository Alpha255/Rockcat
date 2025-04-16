#pragma once

#include "Core/Definitions.h"
#include "Core/Singleton.h"

class IModule
{
public:
	virtual ~IModule() = default;
};

class ModuleFactory : public Singleton<ModuleFactory>
{
public:
	template<class T>
	void RegisterModule(T* Module)
	{
		if (Module && !GetModule<T>())
		{
			m_Modules.emplace_back(std::move(std::unique_ptr<T>(Module)));
			m_Module<T> = Module;
		}
	}

	template<class T>
	T* GetModule()
	{
		return m_Module<T>;
	}

	template<class T>
	bool HasModule()
	{
		return m_Module<T> != nullptr;
	}
protected:
private:
	template<class T>
	inline static T* m_Module = nullptr;

	std::vector<std::unique_ptr<IModule>> m_Modules;
};

template<class T>
inline T& GetModule()
{
	return *(ModuleFactory::Get().GetModule<T>());
}

template<class T>
class IService : public Singleton<T>
{
public:
	virtual ~IService() = default;

	virtual void OnStartup() {}
	virtual void OnShutdown() {}
};