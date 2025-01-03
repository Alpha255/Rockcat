#include "Engine/Engine.h"
#include "Core/PlatformMisc.h"
#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfigurations.h"
#include "Engine/Profile/Profiler.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Services/RenderService.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/Services/FileWatchService.h"
#include "Engine/Services/ShaderCompileService.h"
#include "Engine/Asset/AssetDatabase.h"

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
		Application->Startup();
	}

	while (true)
	{
		if (m_Applications.empty())
		{
			break;
		}

		m_Applications.remove_if([](std::unique_ptr<BaseApplication>& App) {
			if (App->IsRequestQuit())
			{
				App->Shutdown();
				return true;
			}
			return false;
		});

		for (auto& Application : m_Applications)
		{
			//PROFILE_EVENT("%s.Frame", Application->GetConfigurations().GetWindowCreateInfo().Title.c_str());

			Application->Tick(0.0f);

			Application->RenderFrame();
		}
	}

	Finalize();
}

bool Engine::Initialize()
{
	auto AssetsDirectory = PlatformMisc::GetCurrentModuleDirectory().parent_path() / "Assets";
	if (!std::filesystem::exists(AssetsDirectory))
	{
		LOG_ERROR("Invalid assets directory: {}.", AssetsDirectory.string());
		return false;
	}

	PlatformMisc::SetCurrentWorkingDirectory(AssetsDirectory);
	LOG_INFO("Mount working directory to \"{}\".", PlatformMisc::GetCurrentWorkingDirectory().generic_string());

	TaskFlowService::Get().OnStartup();
	AssetDatabase::Get().OnStartup();
	RenderService::Get().OnStartup();
	ShaderCompileService::Get().OnStartup();
	FileWatchService::Get().OnShutdown();

	return true;
}

void Engine::Finalize()
{
	TaskFlowService::Get().OnShutdown();
	AssetDatabase::Get().OnShutdown();
	RenderService::Get().OnShutdown();
	ShaderCompileService::Get().OnShutdown();
	FileWatchService::Get().OnShutdown();
}
