#include "Application/BaseApplication.h"
#include "Application/ApplicationSettings.h"
#include "RHI/RHISwapchain.h"
#include "RHI/RHIUploadManager.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Services/ShaderLibrary.h"
#include "Services/TaskFlowService.h"
#include "Services/AssetDatabase.h"
#include "Profile/CpuTimer.h"
#include "Profile/Stats.h"
#include "Window.h"
#include "System.h"

BaseApplication::BaseApplication(const char* SettingsFile)
	: ITickable(false)
	, EventHandler(EEventMask::WindowStatus)
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

	ShaderLibrary::Create(*m_RenderDevice);
	RHIUploadManager::Create(*m_RenderDevice);

	return true;
}

void BaseApplication::PumpMessages()
{
	System::PumpMessages();
}

bool BaseApplication::IsActivate() const
{
	return m_Window ? m_Window->IsActivate() : true;
}

bool BaseApplication::IsRequestQuit() const
{
	return m_Window ? m_Window->IsDestroyed() : false;
}

void BaseApplication::Tick(float ElapsedSeconds)
{
	TickManager::Get().TickObjects(ElapsedSeconds);
	Stats::Get().Tick(ElapsedSeconds);
}

void BaseApplication::Run()
{
	if (!std::filesystem::exists(Paths::AssetPath()))
	{
		LOG_CRITICAL("Invalid assets directory: {}.", Paths::AssetPath().string());
		return;
	}

	System::SetCurrentWorkingDirectory(Paths::AssetPath());
	LOG_INFO("Mount working directory to \"{}\".", System::GetCurrentWorkingDirectory().string());

	if (!InitializeRHI())
	{
		return;
	}

	TaskFlow::Get().Initialize();
	AssetDatabase::Get().Initialize();

	if (m_Settings->Rendering.Enable)
	{
		m_Window = std::make_unique<Window>(m_Settings->Window);
		assert(m_Window);
	}

	Initialize();

	m_Timer = std::make_unique<CpuTimer>();

	while (true)
	{
		if (IsRequestQuit())
		{
			break;
		}

		if (IsActivate())
		{
			PumpMessages();

			Tick(m_Timer->GetElapsedSeconds());

			if (m_Settings->Rendering.Enable)
			{
				//Render(m_RenderSwapchain ? m_RenderSwapchain->GetBackBuffer() : nullptr);
				RenderGUI();
				//Present();
			}

			TaskFlow::Get().FrameSync(true);
		}
	}

	Finalize();

	AssetDatabase::Get().Finalize();
	TaskFlow::Get().Finalize();

	FinalizeRHI();
}

void BaseApplication::OnWindowStatusChanged(EWindowStatus Status)
{
	switch (Status)
	{
	case EWindowStatus::Activate:
		if (m_Timer)
		{
			m_Timer->Start();
		}
		break;
	case EWindowStatus::Inactivate:
		if (m_Timer)
		{
			m_Timer->Pause();
		}
		break;
	}
}

void BaseApplication::FinalizeRHI()
{
	ShaderLibrary::Destroy();
	RHIUploadManager::Destroy();

	m_RenderDevice.reset();
}

BaseApplication::~BaseApplication() = default;

#if PLATFORM_WIN32
int32_t WINAPI WinMain(_In_ HINSTANCE /*hInstance*/, _In_opt_ HINSTANCE /*hPrevInstance*/, _In_ LPSTR /*Commandline*/, _In_ int32_t /*ShowCmd*/)
{
	extern RunApplication g_RunApplication;

	if (g_RunApplication.CreateApplication)
	{
		auto Application = g_RunApplication.CreateApplication();
		assert(Application);
		Application->Run();
		delete Application;
		return 0;
	}
	else
	{
		LOG_CRITICAL("Failed to create application!");
		return -1;
	}
}
#else
	#error "Unknown platform"
#endif