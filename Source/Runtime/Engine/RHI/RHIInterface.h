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

	virtual ERHIBackend GetRHIType() const { return ERHIBackend::Num; }

	const char* GetName() const { return GetName(GetRHIType()); }

	virtual RHIDevice& GetDevice() = 0;

	const GraphicsSettings& GetGraphicsSettings() const { return GetGraphicsSettings(GetRHIType()); }

	static const char* GetName(ERHIBackend RHI)
	{
		switch (RHI)
		{
		case ERHIBackend::Software: return "Software";
		case ERHIBackend::Vulkan: return "Vulkan";
		case ERHIBackend::D3D11: return "D3D11";
		case ERHIBackend::D3D12: return "D3D12";
		default: return "None";
		}
	}
protected:
	friend class RenderService;

	virtual void InitializeGraphicsDevices() = 0;
	void PrepareStaticResources() {}

	static const GraphicsSettings& GetGraphicsSettings(ERHIBackend Interface);
private:
	static std::array<const GraphicsSettings*, (size_t)ERHIBackend::Num> s_GraphicsSettings;
};