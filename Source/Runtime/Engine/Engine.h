#pragma once

#include "Runtime/Engine/Modules/SpdLogModule.h"
#include "Runtime/Engine/Modules/RenderModule.h"
#include "Runtime/Engine/Modules/TaskFlowModule.h"
#include "Runtime/Engine/Application/BaseApplication.h"

class Engine final : public NoneCopyable
{
public:
	static Engine& Get();

	~Engine() { Finalize(); }

	SpdLogModule& GetSpdLogModule()
	{
		return m_SpdLogModule;
	}

	RenderModule& GetRenderModule()
	{
		return m_RenderModule;
	}

	TaskFlowModule& GetTaskFlowModule()
	{
		return m_TaskFlowModule;
	}

	void Run();

	template<class TApplication>
	struct ApplicationRegister
	{
		ApplicationRegister(const char8_t* ConfigurationName)
		{
			Engine::Get().RegisterApplication<TApplication>(ConfigurationName);
		}
	};

	template<class TApplication>
	void RegisterApplication(const char8_t* ConfigurationName)
	{
		m_Applications.emplace_back(std::make_unique<TApplication>(ConfigurationName));
	}

	template<class TModule>
	TModule& GetModuleByName(const char8_t* ModuleName)
	{
	}

#define GetModule(ModuleType) GetModuleByName<ModuleType>(#ModuleType)
private:
	bool8_t Initialize();
	void Finalize();

	SpdLogModule m_SpdLogModule;
	RenderModule m_RenderModule;
	TaskFlowModule m_TaskFlowModule;

	std::list<std::unique_ptr<BaseApplication>> m_Applications;

	bool8_t m_Initialized = false;
};

#define REGISTER_APPLICATION(ApplicationType, ConfigurationName) \
	static Engine::ApplicationRegister<ApplicationType> CAT(s_##ApplicationType##_, __LINE__)(ConfigurationName);
