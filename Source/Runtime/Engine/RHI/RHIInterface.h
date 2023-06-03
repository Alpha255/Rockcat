#pragma once

#include "Runtime/Core/Definitions.h"
#include "Runtime/Engine/Application/GraphicsSettings.h"

class RHIInterface
{
public:
	RHIInterface(const GraphicsSettings* GfxSettings)
	{
		assert(GfxSettings);
		s_GraphicsSettings = GfxSettings;
	}

	virtual ~RHIInterface() = default;

	virtual ERenderHardwareInterface GetRHIType() const { return ERenderHardwareInterface::Null; }

	static const char8_t* GetRHIName(ERenderHardwareInterface RHI)
	{ 
		switch (RHI)
		{
		case ERenderHardwareInterface::Software: return "Software";
		case ERenderHardwareInterface::Vulkan: return "Vulkan";
		case ERenderHardwareInterface::D3D11: return "D3D11";
		case ERenderHardwareInterface::D3D12: return "D3D12";
		default: return "Null";
		} 
	}

	static const GraphicsSettings* GetGraphicsSettings() { assert(s_GraphicsSettings); return s_GraphicsSettings; }

	const char8_t* GetRHIName() const { return GetRHIName(GetRHIType()); }
protected:
	virtual void InitializeGraphicsDevices() = 0;
private:
	static const GraphicsSettings* s_GraphicsSettings;
};