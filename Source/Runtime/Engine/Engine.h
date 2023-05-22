#pragma once

#include "Runtime/Engine/Modules/SpdLogModule.h"
#include "Runtime/Engine/Modules/RenderModule.h"
#include "Runtime/Engine/Modules/TaskFlowModule.h"

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

	void RunApplication(class BaseApplication& Application);

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

	bool8_t m_Initialized = false;
};

