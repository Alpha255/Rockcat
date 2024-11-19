#include "Engine/Services/RenderService.h"
#include "Engine/Services/SpdLogService.h"
#include "Engine/Engine.h"
#include "Engine/Application/ApplicationConfigurations.h"
#include "RHI/Vulkan/VulkanRHI.h"

void RenderService::InitializeRHI(const GraphicsSettings& GfxSettings)
{
	assert(GfxSettings.RenderHardwareInterface < ERenderHardwareInterface::Num);

	if (!m_RHIs[(size_t)GfxSettings.RenderHardwareInterface])
	{
		const char* RHIName = RHIInterface::GetRHIName(GfxSettings.RenderHardwareInterface);
		switch (GfxSettings.RenderHardwareInterface)
		{
		case ERenderHardwareInterface::Software:
			LOG_ERROR("{} is not support yet!", RHIName);
			break;
		case ERenderHardwareInterface::Vulkan:
			m_RHIs[(size_t)ERenderHardwareInterface::Vulkan] = std::make_unique<VulkanRHI>(&GfxSettings);
			break;
		case ERenderHardwareInterface::D3D11:
			LOG_ERROR("{} is not support yet!", RHIName);
			break;
		case ERenderHardwareInterface::D3D12:
			LOG_ERROR("{} is not support yet!", RHIName);
			break;
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
	for (auto& RHI : m_RHIs)
	{
		RHI.reset();
	}
}
