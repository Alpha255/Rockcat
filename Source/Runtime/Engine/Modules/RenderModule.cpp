#include "Runtime/Engine/Modules/RenderModule.h"
#include "Runtime/Engine/Modules/SpdLogModule.h"
#include "Runtime/Engine/Engine.h"

void RenderModule::InitializeRHI(const GraphicsSettings& GfxSettings)
{
	assert(GfxSettings.RenderHardwareInterface < ERenderHardwareInterface::Null);

	if (!m_RHIs[(size_t)GfxSettings.RenderHardwareInterface])
	{
		switch (GfxSettings.RenderHardwareInterface)
		{
		case ERenderHardwareInterface::Software:
			LOG_ERROR("{} is not support yet!", RHIInterface::GetRHIName(ERenderHardwareInterface::Software));
			break;
		case ERenderHardwareInterface::Vulkan:
			LOG_ERROR("{} is not support yet!", RHIInterface::GetRHIName(ERenderHardwareInterface::Vulkan));
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

void RenderModule::Finalize()
{
}
