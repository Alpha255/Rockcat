#include "Application/Application.h"
#include "Core/Spdlogging.h"
#include "Async/TaskExecutorManager.h"
#include "Asset/AssetDatabase.h"
#include "Profile/CpuTimer.h"

void Application::Loop()
{
	Initialize();

	while (!IsQuit())
	{
		PumpMessages();

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
	LOG_INFO(LogDefault, "Use spdlog @{}", SPDLOG_VERSION);

	TFTaskExecutorManager::Get().Initialize();

	// Initialize Renderbackend

	AssetDatabase::Get().Initialize();
}

void Application::Finalize()
{
	TFTaskExecutorManager::Get().Finalize();
	// Finalize Renderbackend
	// Finalize AssetRegistry
}

void Application::Tick(float ElapsedSeconds)
{
}

void Application::AddMessageHandler(MessageHandler* Handler)
{
	if (Handler &&
		std::find(m_MessageHandlers.begin(), m_MessageHandlers.end(), Handler) == m_MessageHandlers.end())
	{
		m_MessageHandlers.push_back(Handler);
	}
}

void Application::RemoveMessageHandler(MessageHandler* Handler)
{
	if (Handler)
	{
		auto It = std::find(m_MessageHandlers.begin(), m_MessageHandlers.end(), Handler);
		if (It != m_MessageHandlers.end())
		{
			m_MessageHandlers.erase(It);
		}
	}
}

void Application::DispatchKeyUpMessage(const KeyEvent& Event)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Event, this](auto Handler) {
		if (Handler)
		{
			Handler->OnKeyUp(Event, m_KeyModifiers);
		}
	});
}

void Application::DispatchKeyDownMessage(const KeyEvent& Event)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Event, this](auto Handler) {
		if (Handler)
		{
			Handler->OnKeyDown(Event, m_KeyModifiers);
		}
	});
}

void Application::DispatchKeyCharMessage(const char Character)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Character](auto Handler) {
		if (Handler)
		{
			Handler->OnKeyChar(Character);
		}
	});
}

void Application::DispatchMouseButtonUpMessage(const EMouseButton Button)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Button](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseButtonUp(Button);
		}
	});
}

void Application::DispatchMouseButtonDownMessage(const EMouseButton Button, const Math::Vector2& CursorPos)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Button, &CursorPos](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseButtonDown(Button, CursorPos);
		}
	});
}

void Application::DispatchMouseButtonDoubleClickMessage(const EMouseButton Button, const Math::Vector2& CursorPos)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Button, &CursorPos](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseButtonDoubleClick(Button, CursorPos);
		}
	});
}

void Application::DispatchMouseMoveMessage(const Math::Vector2& CursorPos)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&CursorPos](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseMove(CursorPos);
		}
	});
}

void Application::DispatchMouseWheelMessage(float WheelDelta, const Math::Vector2& CursorPos)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&WheelDelta, &CursorPos](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseWheel(WheelDelta, CursorPos);
		}
	});
}

void Application::DispatchMainWindowResizedMessage(uint32_t Width, uint32_t Height)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Width, &Height](auto Handler) {
		if (Handler)
		{
			Handler->OnMainWindowResized(Width, Height);
		}
	});
}

void Application::DispatchAppActiveChangedMessage()
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [](auto Handler) {
		if (Handler)
		{
			Handler->OnAppActive();
		}
	});
}

void Application::DispatchAppInactiveChangedMessage()
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [](auto Handler) {
		if (Handler)
		{
			Handler->OnAppInactive();
		}
	});
}

void Application::DispatchAppQuitMessage()
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [](auto Handler) {
		if (Handler)
		{
			Handler->OnAppQuit();
		}
	});
}

#if PLATFORM_WINDOWS
	#include "Application/Windows/WindowsApplication.h"
	#include "Application/Windows/WindowsPlatformMisc.hpp"
#elif PLATFORM_LINUX
	#include "Application/Linux/LinuxApplication.h"
	#include "Application/Linux/LinuxPlatformMisc.hpp"
#else
	#error "Unknown Platform"
#endif

static std::unique_ptr<Application> CreateApplication()
{
#if PLATFORM_WINDOWS
	return std::make_unique<WindowsApplication>();
#elif PLATFORM_LINUX
	return std::make_unique<LinuxApplication>();
#else
	#error "Unknown Platform"
#endif
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

Application::~Application() = default; // Fix compile error C2027 (unique_ptr)
