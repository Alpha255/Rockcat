#pragma once

#include "Runtime/Core/Definitions.h"
#include "Runtime/Engine/Application/GraphicsSettings.h"

class RHIInterface
{
public:
	virtual ~RHIInterface() = default;

	virtual void Initialize() = 0;

	virtual void Finalize() = 0;

	virtual ERenderHardwareInterface GetRHIType() const { return ERenderHardwareInterface::Null; }

	static const char8_t* GetRHIName(ERenderHardwareInterface RHI)
	{ 
		switch (RHI)
		{
		case ERenderHardwareInterface::Software: return "SoftwareRHI";
		case ERenderHardwareInterface::Vulkan: return "VulkanRHI";
		case ERenderHardwareInterface::D3D11: return "D3D11RHI";
		case ERenderHardwareInterface::D3D12: return "D3D12RHI";
		default: return "Null";
		} 
	}

	const char8_t* GetRHIName() const { return GetRHIName(GetRHIType()); }
};