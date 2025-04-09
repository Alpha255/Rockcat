#include "Engine/Engine.h"
#include "Core/PlatformMisc.h"
#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfiguration.h"
#include "Engine/Profile/Profiler.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Services/TaskFlowService.h"
#include "Engine/Services/ShaderLibrary.h"
#include "Engine/Asset/AssetDatabase.h"
#include "Engine/Paths.h"

void Engine::Run()
{
	Initialize();

	for (auto& Application : m_Applications)
	{
		Application->InitializeRHI();
		Application->Initialize();
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
				App->Finalize();
				return true;
			}
			return false;
		});

		for (auto& Application : m_Applications)
		{
			//PROFILE_EVENT("%s.Frame", Application->GetConfigurations().GetWindowCreateInfo().Title.c_str());

			Application->PumpMessages();

			if (!Application->IsActivate())
			{
				continue;
			}

			Application->Tick(0.0f);

			Application->RenderFrame();
		}

		TaskFlowService::Get().FrameSync(true);
	}

	Finalize();
}

void Engine::Initialize()
{
	if (m_Initialized)
	{
		return;
	}

	if (!std::filesystem::exists(Paths::AssetPath()))
	{
		LOG_CRITICAL("Invalid assets directory: {}.", Paths::AssetPath().string());
		return;
	}

	PlatformMisc::SetCurrentWorkingDirectory(Paths::AssetPath());
	LOG_INFO("Mount working directory to \"{}\".", PlatformMisc::GetCurrentWorkingDirectory().string());

	TaskFlowService::Get().OnStartup();
	AssetDatabase::Get().OnStartup();

	m_Initialized = true;
}

void Engine::Finalize()
{
	TaskFlowService::Get().OnShutdown();
	AssetDatabase::Get().OnShutdown();
}
