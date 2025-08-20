#include "Application/BaseApplication.h"
#include "Application/ApplicationSettings.h"
#include "RHI/RHIBackend.h"
#include "RHI/RHISwapchain.h"
#include "RHI/RHIUploadManager.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "Services/ShaderLibrary.h"
#include "Services/TaskFlowService.h"
#include "Services/AssetDatabase.h"
#include "Profile/CpuTimer.h"
#include "Profile/Stats.h"
#include "Window.h"
#include "System.h"

BaseApplication::BaseApplication(const char* ConfigPath)
	: ITickable(false)
	, EventHandler(EEventMask::WindowStatus)
{ 
	m_Settings = ApplicationSettings::Load(ConfigPath ? ConfigPath : "Defalut.json");
}

bool BaseApplication::InitializeRHI()
{
	switch (m_Settings->GraphicsSettings.Backend)
	{
	case ERHIBackend::Software:
		break;
	case ERHIBackend::Vulkan:
		m_RenderBackend = std::make_unique<VulkanRHI>();
		break;
	case ERHIBackend::D3D11:
		break;
	case ERHIBackend::D3D12:
		break;
	default:
		break;
	}

	if (!m_RenderBackend)
	{
		LOG_CRITICAL("Render backend \"{}\" is not support yet!", RHIBackend::GetName(m_Settings->GraphicsSettings.Backend));
		return false;
	}

	m_RenderBackend->Initialize();
	ShaderLibrary::Create(m_RenderBackend->GetType(), m_RenderBackend->GetDevice());
	RHIUploadManager::Create(m_RenderBackend->GetDevice());
	m_RenderBackend->CreateGlobalResources();
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

void BaseApplication::Present()
{
	if (m_RenderSwapchain)
	{
		m_RenderSwapchain->Present();
	}
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

	TaskFlowService::Get().Initialize();
	AssetDatabase::Get().Initialize();

	if (m_Settings->EnableRendering)
	{
		m_Window = std::make_unique<Window>(m_Settings->WindowDesc);
		assert(m_Window);

		RHISwapchainDesc Desc;
		Desc.SetWindowHandle(m_Window->GetHandle())
			.SetWidth(m_Window->GetWidth())
			.SetHeight(m_Window->GetHeight())
			.SetFullscreen(m_Settings->GraphicsSettings.FullScreen)
			.SetVSync(m_Settings->GraphicsSettings.VSync)
			.SetHDR(m_Settings->GraphicsSettings.HDR);
		m_RenderSwapchain = m_RenderBackend->GetDevice().CreateSwapchain(Desc);
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

			if (m_Settings->EnableRendering)
			{
				Render(m_RenderSwapchain ? m_RenderSwapchain->GetBackBuffer() : nullptr);
				RenderGUI();
				//Present();
			}

			TaskFlowService::Get().FrameSync(true);
		}
	}

	Finalize();

	AssetDatabase::Get().Finalize();
	TaskFlowService::Get().Finalize();

	m_RenderBackend.reset();
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