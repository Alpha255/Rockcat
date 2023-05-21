#pragma once

#include "Runtime/Engine/SpdLogModule.h"

class RenderModule : public IModule
{
};

class TaskFlowModule : public IModule
{
};

class Engine
{
public:
	static Engine& Get();

	RenderModule& GetRenderModule();

	TaskFlowModule& GetTaskFlowModule();

	SpdLogModule& GetSpdLogModule()
	{
		return m_SpdLogModule;
	}

	IModule* GetModule(const char8_t* ModuleName);
protected:
private:
	SpdLogModule m_SpdLogModule;
};

