#pragma once

#include "Engine/Application/BaseApplication.h"

class Engine final : public NoneCopyable
{
public:
	static Engine& Get();

	Engine() = default;
	~Engine() = default;

	void Run();

	template<class TApplication>
	void RegisterApplication(const char* ConfigurationPath)
	{
		m_Applications.emplace_back(std::make_unique<TApplication>(ConfigurationPath));
	}

	template<class TModule>
	TModule& GetModuleByName(const char* ModuleName)
	{
	}

	const std::list<std::unique_ptr<BaseApplication>>& GetApplications() const { return m_Applications; }

#define GetModule(ModuleType) GetModuleByName<ModuleType>(#ModuleType)
private:
	void Initialize();
	void Finalize();

	std::list<std::unique_ptr<BaseApplication>> m_Applications;

	bool m_Initialized = false;
};

template<class TApplication>
struct ApplicationRegister
{
	ApplicationRegister(const char* ConfigurationPath)
	{
		Engine::Get().RegisterApplication<TApplication>(ConfigurationPath);
	}
};

#define REGISTER_APPLICATION(ApplicationType, ConfigurationPath) \
	static ApplicationRegister<ApplicationType> CAT(s_##ApplicationType##_, __LINE__)(ConfigurationPath);

