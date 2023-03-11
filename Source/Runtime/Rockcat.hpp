#pragma once

#include "Core/Gear.h"
#include "Colorful/IRenderer/IRenderer.h"

class Rockcat
{
public:
	template<class TApplication>
	void RunApplication(const char8_t* ApplicationName)
	{
		auto CurrentDirectory = Platform::GetCurrentModuleDirectory();
		auto AssetsDirectory = CurrentDirectory + "\\..\\Assets";
		if (File::Exists(AssetsDirectory.c_str()))
		{
			Platform::SetCurrentWorkingDirectory(AssetsDirectory.c_str());
			LOG_INFO("Mount working directory to \"{}\"", Platform::GetCurrentWorkingDirectory());

			auto Application = std::make_unique<TApplication>();
			const RHI::RenderSettings* Settings = Application->RenderSettings();

			auto MainWindow = std::make_unique<Window>(
				WindowDesc
				{
					Settings->Resolution.Width,
					Settings->Resolution.Height,
					Settings->Resolution.MinWidth,
					Settings->Resolution.MinHeight,
					ApplicationName
				},
				Application.get());

			RHI::IRenderer::Create(Settings, MainWindow->Handle());

			Application->OnStartup();

			CpuTimer GlobalTimer;

			while (MainWindow->State() != Window::EState::Destroyed)
			{
				GlobalTimer.Start();

				MainWindow->PollMessage();

				if (MainWindow->Active())
				{
					Application->Tick(GlobalTimer.ElapsedSeconds());

					Application->Render();
				}
				else
				{
					GlobalTimer.Pause();
					Platform::Sleep(0u);
				}
			}

			RHI::IRenderer::Get().Device()->WaitIdle();

			Application.reset();

			RHI::IRenderer::Destroy();
		}
		else
		{
			LOG_ERROR("Can't find asset directory.");
		}
	}
};

#if defined(PLATFORM_WIN32)
	#define RUN_APPLICATION(Application)                                              \
		int32_t WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int32_t)                \
		{                                                                             \
			VERIFY_PLATFORM(::CoInitializeEx(nullptr, COINIT_MULTITHREADED) == S_OK); \
			Rockcat().RunApplication<Application>(#Application);                      \
			::CoUninitialize();                                                       \
			return 0;                                                                 \
		}
#else
	#error Unknown platform!
#endif