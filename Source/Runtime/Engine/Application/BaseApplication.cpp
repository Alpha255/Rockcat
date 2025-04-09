#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfiguration.h"
#include "RHI/Vulkan/VulkanRHI.h"

BaseApplication::BaseApplication(const char* ConfigurationPath)
{ 
	m_Configs = ApplicationConfiguration::Load(ConfigurationPath ? ConfigurationPath : "Defalut.json");
	MessageRouter::Create();
}

void BaseApplication::InitializeRHI()
{
	if (m_Configs->EnableRendering && !m_RenderBackend)
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
		}

		if (!m_RenderBackend)
		{
			LOG_CRITICAL("Render backend \"{}\" is not support yet!", RHIBackend::GetName(m_Configs->GraphicsSettings.Backend));
		}
		else
		{
			m_RenderBackend->InitializeGraphicsDevice();
			m_RenderBackend->PrepareGlobalResources();
		}

		m_RenderViewport = std::make_unique<Viewport>(
			m_RenderBackend->GetDevice(),
			m_Configs->WindowDesc,
			m_Configs->GraphicsSettings.FullScreen,
			m_Configs->GraphicsSettings.VSync);
	}
}

RHIBackend& BaseApplication::GetRenderBackend()
{
	assert(m_Configs->EnableRendering && m_RenderBackend);
	return *m_RenderBackend;
}

const RenderSettings& BaseApplication::GetRenderSettings() const
{
	return m_Configs->GraphicsSettings;
}

void BaseApplication::PumpMessages()
{
	Window::PumpMessages();
}

bool BaseApplication::IsActivate() const
{
	return m_Configs->EnableRendering ? m_RenderViewport->GetWindow().IsActivate() : true;
}

bool BaseApplication::IsRequestQuit() const
{
	return m_RenderViewport ? m_RenderViewport->GetWindow().IsDestroyed() : false;
}