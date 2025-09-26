#include "Application/BaseApplication.h"
#include "Application/ApplicationSettings.h"
#include "Application/Window.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Services/TaskFlowService.h"
#include "Services/AssetDatabase.h"
#include "Profile/CpuTimer.h"
#include "Profile/Stats.h"
#include "System/System.h"

BaseApplication::BaseApplication(const char* SettingsFile)
{ 
	m_Settings = ApplicationSettings::Load(SettingsFile ? SettingsFile : "Defalut.json");
}

bool BaseApplication::InitializeRHI()
{
	switch (m_Settings->Rendering.DeviceType)
	{
	case ERHIDeviceType::Software:
		break;
	case ERHIDeviceType::Vulkan:
		m_RenderDevice = std::make_unique<VulkanDevice>();
		break;
	case ERHIDeviceType::D3D12:
		break;
	case ERHIDeviceType::D3D11:
		break;
	case ERHIDeviceType::OpenGL:
		break;
	default:
		break;
	}

	if (!m_RenderDevice)
	{
		LOG_CRITICAL("Render backend \"{}\" is not support yet!", RHIDevice::GetName(m_Settings->Rendering.DeviceType));
		return false;
	}

	//ShaderLibrary::Create(*m_RenderDevice);
	//RHIUploadManager::Create(*m_RenderDevice);

	return true;
}

bool BaseApplication::IsActive() const
{
	return true;
}

bool BaseApplication::IsRequestQuit() const
{
	return false;
}

void BaseApplication::RenderFrame()
{
	if (m_Settings->Rendering.Enable)
	{
		Render();
		RenderGUI();

		TaskFlow::Get().FrameSync(true);
	}
}

void BaseApplication::Tick()
{
	auto FrameTime = Stats::Get().GetFrameTime();
	TickManager::Get().TickObjects(FrameTime);
}

void BaseApplication::DispatchKeyUpMessage(const KeyEvent& Event)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Event, this](auto Handler) {
		if (Handler)
		{
			Handler->OnKeyUp(Event, m_KeyModifiers);
		}
	});
}

void BaseApplication::DispatchKeyDownMessage(const KeyEvent& Event)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Event, this](auto Handler) {
		if (Handler)
		{
			Handler->OnKeyDown(Event, m_KeyModifiers);
		}
	});
}

void BaseApplication::DispatchKeyCharMessage(const char Character)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Character](auto Handler) {
		if (Handler)
		{
			Handler->OnKeyChar(Character);
		}
	});
}

void BaseApplication::DispatchMouseButtonUpMessage(const EMouseButton Button)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Button](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseButtonUp(Button);
		}
	});
}

void BaseApplication::DispatchMouseButtonDownMessage(const EMouseButton Button, const Math::Vector2& CursorPos)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Button, &CursorPos](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseButtonDown(Button, CursorPos);
		}
	});
}

void BaseApplication::DispatchMouseButtonDoubleClickMessage(const EMouseButton Button, const Math::Vector2& CursorPos)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Button, &CursorPos](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseButtonDoubleClick(Button, CursorPos);
		}
	});
}

void BaseApplication::DispatchMouseMoveMessage(const Math::Vector2& CursorPos)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&CursorPos](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseMove(CursorPos);
		}
	});
}

void BaseApplication::DispatchMouseWheelMessage(float WheelDelta, const Math::Vector2& CursorPos)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&WheelDelta, &CursorPos](auto Handler) {
		if (Handler)
		{
			Handler->OnMouseWheel(WheelDelta, CursorPos);
		}
	});
}

void BaseApplication::DispatchWindowResizedMessage(uint32_t Width, uint32_t Height)
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [&Width, &Height](auto Handler) {
		if (Handler)
		{
			Handler->OnWindowResized(Width, Height);
		}
	});
}

void BaseApplication::DispatchAppActiveChangedMessage()
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [](auto Handler) {
		if (Handler)
		{
			Handler->OnAppActive();
		}
	});
}

void BaseApplication::DispatchAppInactiveChangedMessage()
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [](auto Handler) {
		if (Handler)
		{
			Handler->OnAppInactive();
		}
	});
}

void BaseApplication::DispatchAppDestroyMessage()
{
	std::for_each(m_MessageHandlers.begin(), m_MessageHandlers.end(), [](auto Handler) {
		if (Handler)
		{
			Handler->OnAppDestroy();
		}
	});
}

void BaseApplication::Run()
{
	if (!std::filesystem::exists(Paths::AssetPath()))
	{
		LOG_CRITICAL("Invalid assets directory: {}.", Paths::AssetPath().string());
		return;
	}

	System::SetWorkingDirectory(Paths::AssetPath());
	LOG_INFO("Mount working directory to \"{}\".", System::GetWorkingDirectory().string());

	if (!InitializeRHI())
	{
		return;
	}

	TaskFlow::Get().Initialize();
	AssetDatabase::Get().Initialize();
	Stats::Get().Initialize();

	if (m_Settings->Rendering.Enable)
	{
		m_Window = std::make_unique<Window>(m_Settings->Window);
		assert(m_Window);
	}

	Initialize();

	while (!IsRequestQuit())
	{
		const bool Active = IsActive();

		Stats::Get().NotifyFrameBegin(Active);

		PumpMessages();

		if (Active)
		{
			Tick();

			RenderFrame();
		}

		Stats::Get().NotifyFrameEnd();
	}

	Finalize();

	AssetDatabase::Get().Finalize();
	TaskFlow::Get().Finalize();
	Stats::Get().Finalize();

	FinalizeRHI();
}

void BaseApplication::FinalizeRHI()
{
	//ShaderLibrary::Destroy();
	//RHIUploadManager::Destroy();

	m_RenderDevice.reset();
}

BaseApplication::~BaseApplication() = default;

#if PLATFORM_WIN32
int32_t WINAPI WinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*Commandline*/, _In_ int32_t /*ShowCmd*/)
{
	//extern RunApplication g_RunApplication;

	//if (g_RunApplication.CreateApplication)
	//{
	//	auto Application = g_RunApplication.CreateApplication();
	//	assert(Application);
	//	Application->Run();
	//	delete Application;
	//	return 0;
	//}
	//else
	//{
	//	LOG_CRITICAL("Failed to create application!");
	//	return -1;
	//}

	return 0;
}
#else
	#error "Unknown platform"
#endif