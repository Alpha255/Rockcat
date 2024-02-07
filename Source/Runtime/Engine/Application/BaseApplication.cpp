#if 0
#include "Colorful/IRenderer/RenderGraph/SimpleFrameGraph.h"

RHI::IRenderer* Application::GRenderer = nullptr;

Application::Application(const char* SettingPath)
	: m_RenderSettings(RHI::RenderSettings::Load(SettingPath))
{
}

void Application::Render()
{
	if (!GRenderer)
	{
		GRenderer = &RHI::IRenderer::Get();
	}

	if (GRenderer)
	{
		GRenderer->AdvanceNextFrame();

		RenderScene(GRenderer->SwapchainFrameBuffer());

		GRenderer->Present();
	}
}

void Application::RenderScene(RHI::IFrameBuffer* SwapchainFrameBuffer)
{
	if (m_Scene)
	{
		if (!m_FrameGraph || m_FrameGraph && m_FrameGraph->RenderingPath() != m_RenderSettings->RenderingPath)
		{
			m_FrameGraph = std::make_unique<RHI::SimpleFrameGraph>(m_RenderSettings->RenderingPath, m_FrameGraph.get());

			RHI::ForwardShadingPass* ForwardShadingPass = nullptr;

			switch (m_RenderSettings->RenderingPath)
			{
			case RHI::ERenderingPath::ForwardShading:
				ForwardShadingPass = m_FrameGraph->AddPass<RHI::ForwardShadingPass>(m_Scene.get(), m_Camera);
				break;
			case RHI::ERenderingPath::DeferredShading:
				m_FrameGraph->AddPass<RHI::GBufferPass>(m_Scene.get(), m_Camera);
				m_FrameGraph->AddPass<RHI::DeferredLightingPass>(m_Scene.get(), m_Camera);
				break;
			}

			m_FrameGraph->AddPass<RHI::SkyPass>(m_Scene.get(), m_Camera);
			m_FrameGraph->AddPass<RHI::PostProcessingPass>(m_Scene.get(), m_Camera);
			auto BlitPass = m_FrameGraph->AddPass<RHI::BlitPass>(m_Scene.get(), m_Camera);
			m_GUIPass = m_FrameGraph->AddPass<RHI::ImGUIPass>(m_Scene.get(), m_Camera);

			m_GUIPass->BindBuildWidgetsFunc([this]() {
				this->RenderGUI();
				});

			BlitPass->BindPass(ForwardShadingPass);
		}

		m_FrameGraph->Render(m_RenderSettings.get(), SwapchainFrameBuffer);
	}
}

void Application::OnMouseEvent(const MouseEvent& Mouse)
{
	if (m_Camera)
	{
		if (!m_GUIPass || !m_GUIPass->WantCaptureInput())
		{
			m_Camera->OnMouseEvent(Mouse);
		}
	}

	if (m_GUIPass)
	{
		m_GUIPass->OnMouseEvent(Mouse);
	}
}

void Application::OnKeyboardEvent(const KeyboardEvent& Keyboard)
{
	if (m_Camera)
	{
		if (!m_GUIPass || !m_GUIPass->WantCaptureInput())
		{
			m_Camera->OnKeyboardEvent(Keyboard);
		}
	}

	if (m_GUIPass)
	{
		m_GUIPass->OnKeyboardEvent(Keyboard);
	}
}

void Application::OnWindowResized(uint32_t Width, uint32_t Height)
{
	if (m_Camera)
	{
		m_Camera->OnWindowResized(Width, Height);
	}

	if (m_GUIPass)
	{
		m_GUIPass->OnWindowResized(Width, Height);
	}

	if (GRenderer)
	{
		GRenderer->OnWindowResized(Width, Height);
	}
}

void Application::Tick(float ElapsedSeconds)
{
	if (m_Scene)
	{
		m_Scene->Tick(ElapsedSeconds);
	}

	if (m_Camera)
	{
		m_Camera->Tick(ElapsedSeconds);
	}

	if (m_GUIPass)
	{
		m_GUIPass->Tick(ElapsedSeconds);
	}

	RHI::Profiler::Stats::Get().Tick(ElapsedSeconds);
}

Application::~Application()
{
	if (m_Scene)
	{
		m_Scene->Save();
	}
}
#endif

#include "Engine/Application/BaseApplication.h"
#include "Engine/Application/ApplicationConfigurations.h"
#include "Core/Window.h"
#include "Engine/Services/RenderService.h"

BaseApplication::BaseApplication(const char* ConfigurationName)
{ 
	m_Configs = ApplicationConfigurations::Load(ConfigurationName ? ConfigurationName : "Defalut.json");
}

void BaseApplication::OnStartup()
{
	OnWindowCreation();
	OnInitialize();
}

void BaseApplication::OnWindowCreation()
{
	if (m_Configs->IsEnableWindow())
	{
		m_Window = std::make_shared<Window>(m_Configs->GetWindowCreateInfo(), nullptr);
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

bool BaseApplication::IsRequestingQuit() const
{
	return m_Window ? m_Window->IsDestroyed() : false;
}

RHIInterface& BaseApplication::GetRHI()
{
	return RenderService::Get().GetRHIInterface(m_Configs->GetGraphicsSettings().RenderHardwareInterface);
}