#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfiguration.h"
#include "Engine/Services/RenderService.h"
#include "Core/Window.h"

BaseApplication::BaseApplication(const char* ConfigurationPath)
{ 
	m_Configs = ApplicationConfiguration::Load(ConfigurationPath ? ConfigurationPath : "Defalut.json");
}

void BaseApplication::SetRenderBackend(class RHIBackend* Backend)
{
	if (m_Configs->EnableRendering && !m_RenderBackend)
	{
		m_RenderBackend = Backend;

		if (m_Configs->EnableRendering)
		{
			m_RenderViewport = std::make_unique<Viewport>(
				GetRenderBackend(), 
				m_Configs->WindowDesc, 
				m_Configs->GraphicsSettings.FullScreen, 
				m_Configs->GraphicsSettings.VSync);
		}
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
#if PLATFORM_WIN32
	::MSG Message;
	if (::PeekMessageA(&Message, nullptr, 0u, 0u, PM_REMOVE))
	{
		::TranslateMessage(&Message);
		::DispatchMessageA(&Message);
	}
#else
	assert(0);
#endif
}

bool BaseApplication::IsRequestQuit() const
{
	return m_RenderViewport ? m_RenderViewport->GetWindow().IsDestroyed() : false;
}