#include "Runtime/Engine/Engine.h"
#include "Runtime/Core/PlatformMisc.h"
#include "Runtime/Engine/Application/BaseApplication.h"
#include "Runtime/Engine/Application/ApplicationConfigurations.h"
#include "Runtime/Engine/Profile/CpuTimer.h"
#include "Runtime/Engine/Services/SpdLogService.h"
#include "Runtime/Engine/Services/RenderService.h"
#include "Runtime/Engine/Services/TaskFlowService.h"
#include "Runtime/Engine/Asset/AssetDatabase.h"
#include "Runtime/Engine/Asset/ShaderCompiler.h"

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

	for (auto& Application : m_Applications)
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

		for (auto& Application : m_Applications)
		{
			Application->Tick(0.0f);
		}
	}

	Finalize();
}

bool8_t Engine::Initialize()
{
	auto AssetsDirectory = PlatformMisc::GetCurrentModuleDirectory().parent_path() / "Assets";
	if (!std::filesystem::exists(AssetsDirectory))
	{
		LOG_ERROR("Invalid assets directory.");
		return false;
	}

	PlatformMisc::SetCurrentWorkingDirectory(AssetsDirectory);
	LOG_INFO("Mount working directory to \"{}\".", PlatformMisc::GetCurrentWorkingDirectory().generic_string());

	for (auto& Application : m_Applications)
	{
		if (Application->GetConfigurations().IsEnableRendering())
		{
			RenderService::Get().InitializeRHI(Application->GetConfigurations().GetGraphicsSettings());
		}
	}

	TaskFlowService::Get().OnStartup();
	AssetDatabase::Get().OnStartup();
	RenderService::Get().OnStartup();
	ShaderCompileService::Get().OnStartup();

	return true;
}

void Engine::Finalize()
{
	TaskFlowService::Get().OnShutdown();
	AssetDatabase::Get().OnShutdown();
	RenderService::Get().OnShutdown();
	ShaderCompileService::Get().OnShutdown();
}
