#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfiguration.h"
#include "Engine/Application/Viewport.h"
#include "Core/Window.h"
#include "Core/System.h"

BaseApplication::~BaseApplication() = default;

BaseApplication::BaseApplication(const char* ConfigurationPath)
{ 
	m_Configs = ApplicationConfiguration::Load(ConfigurationPath ? ConfigurationPath : "Defalut.json");
}

void BaseApplication::CreateWindowAndViewport()
{
	if (m_Configs->EnableRendering)
	{
		m_Window = std::make_unique<Window>(m_Configs->WindowDesc, this);
		m_Viewport = std::make_unique<Viewport>(*m_Window, m_Configs->GraphicsSettings.FullScreen, m_Configs->GraphicsSettings.VSync);
	}
}

const RenderSettings& BaseApplication::GetRenderSettings() const
{
	return m_Configs->GraphicsSettings;
}

void BaseApplication::PumpMessages()
{
	System::PumpMessages();
}

#if PLATFORM_WIN32
	#include "Core/Win32/Win32DynamicLinkLibrary.hpp"
	#include "Core/Win32/Win32System.hpp"
	#include "Core/Win32/Win32Window.hpp"
	#include "Engine/Application/Win32/Win32BaseApplication.hpp"
#else
	#error "Unknown platform"
#endif