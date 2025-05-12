#include "Application/BaseApplication.h"
#include "Application/ApplicationConfiguration.h"
#include "RHI/RHIBackend.h"
#include "RHI/RHIUploadManager.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "Services/ShaderLibrary.h"
#include "Profile/CpuTimer.h"
#include "Window.h"
#include "System.h"

BaseApplication::BaseApplication(const char* ConfigPath)
{ 
	m_Configs = ApplicationConfiguration::Load(ConfigPath ? ConfigPath : "Defalut.json");
}

bool BaseApplication::InitializeRHI()
{
	switch (m_Configs->GraphicsSettings.Backend)
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
		LOG_CRITICAL("Render backend \"{}\" is not support yet!", RHIBackend::GetName(m_Configs->GraphicsSettings.Backend));
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

	if (m_Configs->EnableRendering)
	{
		m_Window = std::make_unique<Window>(m_Configs->WindowDesc);
		assert(m_Window);
		//RenderService::Get().GetBackend().CreateSwapchain(*m_Window, m_Configs->GraphicsSettings);
	}

	Initialize();

	m_Timer = std::make_unique<CpuTimer>();
}

#if PLATFORM_WIN32
	#include "Win32/Win32DynamicLinkLibrary.hpp"
	#include "Win32/Win32System.hpp"
	#include "Win32/Win32Window.hpp"
#else
	#error "Unknown platform"
#endif