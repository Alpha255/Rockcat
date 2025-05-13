#pragma once

#include "Core/Definitions.h"
#include "Rendering/RenderSettings.h"
#include "RHI/RHIDevice.h"

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
	virtual ~RHIBackend() = default;
	virtual ERHIBackend GetType() const { return ERHIBackend::Num; }
	virtual RHIDevice& GetDevice() = 0;

	const char* GetName() const { return GetName(GetType()); }

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
	friend class BaseApplication;

	virtual void Initialize() = 0;
	void CreateGlobalResources() {}
};