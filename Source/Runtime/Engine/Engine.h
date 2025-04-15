#pragma once

#include "Core/Singleton.h"
#include "Engine/Application/BaseApplication.h"

class Engine final : public Singleton<Engine>
{
public:
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
private:
	void Initialize();
	void Finalize();

	std::list<std::unique_ptr<BaseApplication>> m_Applications;
	std::shared_ptr<struct RenderingConfiguration> m_RenderingConfigs;

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

