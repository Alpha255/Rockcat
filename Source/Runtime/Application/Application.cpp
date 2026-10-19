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
	#include "Application/Windows/WindowsPlatformMisc.hpp"
#elif PLATFORM_LINUX
	#include "Application/Linux/LinuxApplication.h"
	#include "Application/Linux/LinuxPlatformMisc.hpp"
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

Application::Application()
{
	m_Handle = PlatformMisc::GetApplicationHandle();
}

string GetSystemErrorMessage(uint32_t ErrorCode)
{
	return PlatformMisc::GetSystemErrorMessage(ErrorCode);
}

std::filesystem::path Application::GetExecutablePath()
{
	return PlatformMisc::GetExecutablePath();
}

std::filesystem::path Application::GetWorkingDirectory()
{
	return PlatformMisc::GetWorkingDirectory();
}

void Application::SetWorkingDirectory(const std::filesystem::path& Directory)
{
	PlatformMisc::SetWorkingDirectory(Directory);
}

void Application::Sleep(uint32_t Seconds)
{
	PlatformMisc::Sleep(Seconds);
}

string Application::GetEnvironmentVariable(const char* Name)
{
	return PlatformMisc::GetEnvironmentVariable(Name);
}

void Application::SetEnvironmentVariable(const char* Name, const char* Value)
{
	PlatformMisc::SetEnvironmentVariable(Name, Value);
}

string Application::GetCommandline()
{
	return PlatformMisc::GetCommandline();
}

void Application::ExecuteProcess(const char* Commandline, bool WaitDone = true)
{
	return PlatformMisc::ExecuteProcess(Commandline, WaitDone);
}

uint32_t Application::GetNumHardwareConcurrencyThreads(bool UseHyperThreading)
{
	return PlatformMisc::GetNumHardwareConcurrencyThreads(UseHyperThreading);
}

void Application::SetThreadPriority(std::thread::id ThreadID, ETFTaskPriority Priority)
{
	return PlatformMisc::SetThreadPriority(ThreadID, Priority);
}
