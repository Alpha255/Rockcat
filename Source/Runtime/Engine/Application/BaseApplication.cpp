#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfiguration.h"
#include "Engine/Services/RenderService.h"
#include "Engine/RHI/RHIViewport.h"
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