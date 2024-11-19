#pragma once

#include "Core/Definitions.h"
#include "Engine/Application/GraphicsSettings.h"
#include "Engine/RHI/RHIDevice.h"

/// <summary>
/// ****  Front face CCW
/// ****  Matrix is in row major in Cpp
/// ****  Matrix is in row major in shader
/// ****  Cull back face
/// ****  Left-hand coordinate
/// ****  Depth range [0-1]
/// </summary>

class RHIInterface
{
public:
	RHIInterface(const GraphicsSettings* GfxSettings);

	virtual ~RHIInterface() = default;

	virtual ERenderHardwareInterface GetRHIType() const { return ERenderHardwareInterface::Num; }

	const char* GetName() const { return GetRHIName(GetRHIType()); }

	virtual RHIDevice& GetDevice() = 0;

	static const char* GetRHIName(ERenderHardwareInterface RHI)
	{
		switch (RHI)
		{
		case ERenderHardwareInterface::Software: return "Software";
		case ERenderHardwareInterface::Vulkan: return "Vulkan";
		case ERenderHardwareInterface::D3D11: return "D3D11";
		case ERenderHardwareInterface::D3D12: return "D3D12";
		default: return "None";
		}
	}
protected:
	virtual void InitializeGraphicsDevices() = 0;

	static const GraphicsSettings& GetGraphicsSettings(ERenderHardwareInterface Interface);
private:
	static std::array<const GraphicsSettings*, (size_t)ERenderHardwareInterface::Num> s_GraphicsSettings;
};