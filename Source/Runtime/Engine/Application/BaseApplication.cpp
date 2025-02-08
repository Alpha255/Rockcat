#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfiguration.h"
#include "Engine/Services/RenderService.h"
#include "Engine/RHI/RHISwapchain.h"
#include "Core/Window.h"

BaseApplication::BaseApplication(const char* ConfigurationPath)
{ 
	m_Configs = ApplicationConfiguration::Load(ConfigurationPath ? ConfigurationPath : "Defalut.json");
}

void BaseApplication::Startup()
{
	MakeWindow();
	Initialize();
}

void BaseApplication::MakeWindow()
{
	if (m_Configs->EnableWindow)
	{
		m_Window = std::make_shared<Window>(m_Configs->WindowDesc, this);

		if (m_Configs->EnableRendering)
		{
			assert(m_RenderBackend);

			RHISwapchainCreateInfo CreateInfo;
			CreateInfo.SetWindowHandle(m_Window->GetHandle())
				.SetWidth(m_Window->GetWidth())
				.SetHeight(m_Window->GetHeight())
				.SetFullscreen(GetRenderSettings().FullScreen)
				.SetVSync(GetRenderSettings().VSync);
			m_Swapchain = m_RenderBackend->GetDevice().CreateSwapchain(CreateInfo);
		}
	}
}

void BaseApplication::Tick(float /*ElapsedSeconds*/)
{
	if (m_Window)
	{
		m_Window->PollMessage();
	}
}

void BaseApplication::SetRenderBackend(const std::shared_ptr<RHIBackend>& Backend)
{
	if (m_Configs->EnableRendering && !m_RenderBackend)
	{
		m_RenderBackend = Backend;
	}
}

RHIBackend& BaseApplication::GetRenderBackend()
{
	assert(m_Configs->EnableRendering && m_RenderBackend);
	return *m_RenderBackend;
}

RHISwapchain& BaseApplication::GetRenderSwapchain()
{
	assert(m_Configs->EnableRendering && m_Swapchain);
	return *m_Swapchain;
}

const Window& BaseApplication::GetWindow()
{
	assert(m_Configs->EnableWindow);
	return *m_Window;
}

const RenderSettings& BaseApplication::GetRenderSettings() const
{
	return m_Configs->GraphicsSettings;
}

bool BaseApplication::IsRequestQuit() const
{
	return m_Window ? m_Window->IsDestroyed() : false;
}