#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfigurations.h"
#include "Core/Window.h"
#include "Engine/Services/RenderService.h"

BaseApplication::BaseApplication(const char* ConfigurationName)
{ 
	m_Configs = ApplicationConfigurations::Load(ConfigurationName ? ConfigurationName : "Defalut.json");
}

void BaseApplication::Startup()
{
	MakeWindow();
	Initialize();
}

void BaseApplication::MakeWindow()
{
	if (m_Configs->IsEnableWindow())
	{
		m_Window = std::make_shared<Window>(m_Configs->GetWindowCreateInfo(), this);
	}
}

void BaseApplication::Tick(float /*ElapsedSeconds*/)
{
	if (m_Window)
	{
		m_Window->PollMessage();
	}
}

const ApplicationConfigurations& BaseApplication::GetConfigurations() const
{ 
	return *m_Configs;
}

const GraphicsSettings& BaseApplication::GetGraphicsSettings() const
{
	return m_Configs->GetGraphicsSettings();
}

bool BaseApplication::IsRequestQuit() const
{
	return m_Window ? m_Window->IsDestroyed() : false;
}