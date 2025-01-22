#include "Engine/Services/RenderService.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Engine.h"
#include "Engine/Application/ApplicationConfigurations.h"
#include "RHI/Vulkan/VulkanRHI.h"

void RenderService::InitializeRHI(const GraphicsSettings& GfxSettings)
{
	assert(GfxSettings.Interface < ERHIBackend::Num);

	if (!m_Backends[GfxSettings.Interface])
	{
		switch (GfxSettings.Interface)
		{
		case ERHIBackend::Software:
			break;
		case ERHIBackend::Vulkan:
			m_Backends[ERHIBackend::Vulkan] = std::make_unique<VulkanRHI>(&GfxSettings);
			break;
		case ERHIBackend::D3D11:
			break;
		case ERHIBackend::D3D12:
			break;
		}

		if (!m_Backends[GfxSettings.Interface])
		{
			LOG_CRITICAL("{} is not support yet!", RHIInterface::GetName(GfxSettings.Interface));
		}
		else
		{
			m_Backends[GfxSettings.Interface]->PrepareStaticResources();
		}
	}
}

void RenderService::OnStartup()
{
	for (auto& Application : Engine::Get().GetApplications())
	{
		if (Application->GetConfigurations().IsEnableRendering())
		{
			InitializeRHI(Application->GetConfigurations().GetGraphicsSettings());
		}
	}
}

void RenderService::OnShutdown()
{
	for (auto& Backend : m_Backends)
	{
		Backend.reset();
	}
}
