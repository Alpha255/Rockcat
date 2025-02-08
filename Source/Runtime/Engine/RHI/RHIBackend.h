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

class RHIBackend
{
public:
	RHIBackend();

	virtual ~RHIBackend() = default;
	virtual ERHIBackend GetType() const { return ERHIBackend::Num; }
	virtual RHIDevice& GetDevice() = 0;

	const char* GetName() const { return GetName(GetType()); }

	static const RHIBackendConfiguration& GetConfigs() { return *s_Configs; }

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

	virtual void InitializeGraphicsDevice() = 0;
	void PrepareGlobalResources() {}

	static std::shared_ptr<RHIBackendConfiguration> s_Configs;
};