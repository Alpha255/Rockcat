#pragma once

#include "Runtime/Engine/Application/BaseApplication.h"

class Engine final : public NoneCopyable
{
public:
	static Engine& Get();

	Engine() = default;

	~Engine() = default;

	void Run();

	template<class TApplication>
	struct ApplicationRegister
	{
		ApplicationRegister(const char* ConfigurationName)
		{
			Engine::Get().RegisterApplication<TApplication>(ConfigurationName);
		}
	};

	template<class TApplication>
	void RegisterApplication(const char* ConfigurationName)
	{
		m_Applications.emplace_back(std::make_unique<TApplication>(ConfigurationName));
	}

	template<class TModule>
	TModule& GetModuleByName(const char* ModuleName)
	{
	}

#define GetModule(ModuleType) GetModuleByName<ModuleType>(#ModuleType)
private:
	bool8_t Initialize();
	void Finalize();

	std::list<std::unique_ptr<BaseApplication>> m_Applications;

	bool8_t m_Initialized = false;
};

#define REGISTER_APPLICATION(ApplicationType, ConfigurationName) \
	static Engine::ApplicationRegister<ApplicationType> CAT(s_##ApplicationType##_, __LINE__)(ConfigurationName);

