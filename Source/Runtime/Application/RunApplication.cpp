#include "Application/RunApplication.h"
#include "Application/BaseApplication.h"
#include "Application/ApplicationConfiguration.h"
#include "Services/SpdLogService.h"
#include "Services/TaskFlowService.h"
#include "Services/RenderService.h"
#include "Asset/AssetDatabase.h"
#include "RHI/RHIBackend.h"
#include "Paths.h"
#include "System.h"

void ApplicationRunner::Run()
{
	Initialize();
	GApplication->Initialize();

	GTimer.reset(new CpuTimer());

	while (true)
	{
		if (GApplication->IsRequestQuit())
		{
			break;
		}

		if (GApplication->IsActivate())
		{
			GTimer->Start();
			
			GApplication->PumpMessages();

			TickManager::Get().TickObjects(GTimer->GetElapsedSeconds());

			if (GApplication->GetConfigs().EnableRendering)
			{
				GApplication->RenderFrame();

				RenderService::Get().GetBackend().AdvanceFrame();
			}

			TaskFlowService::Get().FrameSync(true);
		}
		else
		{
			GTimer->Pause();
		}
	}

	GApplication->Finalize();
	Finalize();
}

void ApplicationRunner::Initialize()
{
	if (!std::filesystem::exists(Paths::AssetPath()))
	{
		LOG_CRITICAL("Invalid assets directory: {}.", Paths::AssetPath().string());
		return;
	}

	System::SetCurrentWorkingDirectory(Paths::AssetPath());
	LOG_INFO("Mount working directory to \"{}\".", System::GetCurrentWorkingDirectory().string());

	TaskFlowService::Get().OnStartup();
	AssetDatabase::Get().OnStartup();
	RenderService::Get().OnStartup();
}

void ApplicationRunner::Finalize()
{
	TaskFlowService::Get().OnShutdown();
	AssetDatabase::Get().OnShutdown();
	RenderService::Get().OnShutdown();
}

#if defined(PLATFORM_WIN32)
int32_t WINAPI WinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*Commandline*/, _In_ int32_t /*ShowCmd*/)
#else
int32_t main(int32_t /*Argc*/, char** /*Argv*/)
#endif
{
	extern ApplicationRunner GApplicationRunner;

	GApplicationRunner.Run();

	return 0;
}
