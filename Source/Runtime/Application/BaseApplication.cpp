#include "Application/BaseApplication.h"
#include "Application/ApplicationConfiguration.h"
#include "Window.h"
#include "System.h"

BaseApplication::~BaseApplication() = default;

BaseApplication::BaseApplication(const char* ConfigurationPath)
{ 
	m_Configs = ApplicationConfiguration::Load(ConfigurationPath ? ConfigurationPath : "Defalut.json");
}

void BaseApplication::Initialize()
{
	if (m_Configs->EnableRendering)
	{
		m_Window = std::make_unique<Window>(m_Configs->WindowDesc, this);
	}

	InitializeImpl();
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
	#include "Win32/Win32DynamicLinkLibrary.hpp"
	#include "Win32/Win32System.hpp"
	#include "Win32/Win32Window.hpp"
	#include "Application/Win32/Win32BaseApplication.hpp"
#else
	#error "Unknown platform"
#endif