#include "Runtime/Engine/Engine.h"
#include "Runtime/Core/PlatformMisc.h"
#include "Runtime/Engine/Application/BaseApplication.h"
#include "Runtime/Engine/Application/ApplicationConfigurations.h"
#include "Runtime/Engine/Profile/CpuTimer.h"

Engine& Engine::Get()
{
	static Engine s_Engine;
	return s_Engine;
}

void Engine::Run()
{
	if (!m_Initialized)
	{
		m_Initialized = Initialize();
		assert(m_Initialized);
	}

	for each (auto& Application in m_Applications)
	{
		Application->OnStartup();
	}

	while (true)
	{
		if (m_Applications.empty())
		{
			break;
		}

		m_Applications.remove_if([](std::unique_ptr<BaseApplication>& App) {
			if (App->IsRequestingQuit())
			{
				App->OnShutdown();
				return true;
			}
			return false;
		});

		for each (auto& Application in m_Applications)
		{
			Application->Tick(0.0f);
		}
	}
}

bool8_t Engine::Initialize()
{
	auto AssetsDirectory = PlatformMisc::GetCurrentModuleDirectory() + "\\..\\Assets";
	if (!std::filesystem::exists(AssetsDirectory))
	{
		LOG_ERROR("Invalid assets directory.");
		return false;
	}

	PlatformMisc::SetCurrentWorkingDirectory(AssetsDirectory.c_str());
	LOG_INFO("Mount working directory to \"{}\".", PlatformMisc::GetCurrentWorkingDirectory());

	for each (auto& Application in m_Applications)
	{
		if (Application->GetConfigurations().IsEnableRendering())
		{
			m_RenderModule.InitializeRHI(Application->GetConfigurations().GetGraphicsSettings());
		}
	}

	return true;
}

void Engine::Finalize()
{
	m_RenderModule.Finalize();
}
