#include "Runtime/Engine/Services/RenderService.h"
#include "Runtime/Engine/Services/SpdLogService.h"
#include "RHI/Vulkan/VulkanRHI.h"

void RenderService::InitializeRHI(const GraphicsSettings& GfxSettings)
{
	assert(GfxSettings.RenderHardwareInterface < ERenderHardwareInterface::Num);

	if (!m_RHIs[(size_t)GfxSettings.RenderHardwareInterface])
	{
		switch (GfxSettings.RenderHardwareInterface)
		{
		case ERenderHardwareInterface::Software:
			LOG_ERROR("{} is not support yet!", RHIInterface::GetRHIName(ERenderHardwareInterface::Software));
			break;
		case ERenderHardwareInterface::Vulkan:
			m_RHIs[(size_t)ERenderHardwareInterface::Vulkan] = std::make_unique<VulkanRHI>(&GfxSettings);
			break;
		case ERenderHardwareInterface::D3D11:
			LOG_ERROR("{} is not support yet!", RHIInterface::GetRHIName(ERenderHardwareInterface::D3D11));
			break;
		case ERenderHardwareInterface::D3D12:
			LOG_ERROR("{} is not support yet!", RHIInterface::GetRHIName(ERenderHardwareInterface::D3D12));
			break;
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
