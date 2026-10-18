#include "Application/Application.h"

void Application::Loop()
{
	Initialize();

	while (!IsQuit())
	{
#if WITH_EDITOR
		//m_Editor->Tick(0.0f);
#else
		//m_GameInstance->Tick(0.0f);
#endif
	}

	Finalize();
}

void Application::Initialize()
{
	// Initialize TaskExecutorManager
	// Initialize Renderbackend
	// Initialize AssetRegistry
}

void Application::Finalize()
{
	// Finalize TaskExecutorManager
	// Finalize Renderbackend
	// Finalize AssetRegistry
}

void Application::Tick(float ElapsedSeconds)
{
}

#if PLATFORM_WINDOWS
	#include "Application/Windows/WindowsApplication.h"
#elif PLATFORM_LINUX
#endif

std::unique_ptr<Application> CreateApplication()
{
}

Application& Application::Get()
{
	static std::unique_ptr<Application> GApplication;
	if (!GApplication)
	{
		GApplication = std::move(CreateApplication());
	}

	return *GApplication;
}
