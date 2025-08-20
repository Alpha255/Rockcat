#pragma once

#include "RHI/RHICommandBuffer.h"
#include "Services/SpdLogService.h"

/// <summary>
/// ****  Front face CCW
/// ****  Matrix is in row major in Cpp
/// ****  Matrix is in row major in shader
/// ****  Cull back face
/// ****  Left-hand coordinate
/// ****  Depth range [0-1]
/// </summary>

enum class ERHIDeviceQueue : uint8_t
{
	Graphics,
	Transfer,
	Compute,
	Num
};

struct RHIDeviceCapabilities
{
	uint32_t MaxTextureDimension1D = 0u;
	uint32_t MaxTextureDimension2D = 0u;
	uint32_t MaxTextureDimension3D = 0u;
	uint32_t MaxTextureDimensionCube = 0u;
	uint32_t MaxTextureArrayLayers = 0u;
	uint32_t MaxInputBindings = 0u;
	uint32_t MaxInputAttributes = 0u;
	uint32_t MaxComputeWorkGroupCountX = 0u;
	uint32_t MaxComputeWorkGroupCountY = 0u;
	uint32_t MaxComputeWorkGroupCountZ = 0u;
	uint32_t MaxViewports = 0u;
	uint32_t MaxFrameBufferWidth = 0u;
	uint32_t MaxFrameBufferHeight = 0u;
	uint32_t MaxFrameBufferLayers = 0u;
	uint32_t MaxColorAttachments = 0u;
	uint32_t MaxDrawIndexedIndexValue = 0;
	uint32_t MaxDrawIndirectNum = 0u;
	uint32_t MaxSamplersPerStage = 0u;
	uint32_t MaxResourcesPerStage = 0u;
	uint32_t MaxDescriptorSets = 0u;
	float MaxSamplerAnisotropy = 0.0f;

	bool SupportsAsyncCompute = false;
	bool SupportsTransferQueue = false;
};

class RHIDevice
{
public:
	enum class EVendorID
	{
		AMD = 0x1002,
		ImgTec = 0x1010,
		Nvidia = 0x10DE,
		ARM = 0x13B5,
		Qualcomm = 0x5143,
		Intel = 0x8086,
		MiscrosoftWrap = 0x1414
	};

	template<class TID>
	static const char* const GetVendorName(TID VendorID)
	{
		switch (static_cast<EVendorID>(VendorID))
		{
		case EVendorID::AMD: return "AMD";
		case EVendorID::ImgTec: return "ImgTec";
		case EVendorID::Nvidia: return "Nvidia";
		case EVendorID::ARM: return "ARM";
		case EVendorID::Qualcomm: return "Qualcomm";
		case EVendorID::Intel: return "Intel";
		case EVendorID::MiscrosoftWrap: return "Miscrosoft Wrap";
		default: return "Unknown";
		}
	}

	template<class TID>
	static bool IsDedicated(TID VendorID)
	{
		return static_cast<EVendorID>(VendorID) == EVendorID::AMD ||
			static_cast<EVendorID>(VendorID) == EVendorID::Nvidia ||
			static_cast<EVendorID>(VendorID) == EVendorID::Intel;
	}

	static const char* const GetDeviceName(ERHIDeviceType Type)
	{
		switch (Type)
		{
		case ERHIDeviceType::Software: return "Software";
		case ERHIDeviceType::Vulkan: return "Vulkan";
		case ERHIDeviceType::D3D12: return "D3D12";
		case ERHIDeviceType::D3D11: return "D3D11";
		case ERHIDeviceType::OpenGL: return "OpenGL";
		default: return "Unknown";
		}
	}

	const char* GetName() const { return GetDeviceName(GetType()); }

	virtual ERHIDeviceType GetType() const = 0;

	virtual void WaitIdle() const = 0;

	virtual RHIShaderPtr CreateShader(const RHIShaderDesc& Desc) = 0;
	virtual RHITexturePtr CreateTexture(const RHITextureDesc& Desc) = 0;
	virtual RHIInputLayoutPtr CreateInputLayout(const RHIInputLayoutDesc& Desc) = 0;
	virtual RHIFrameBufferPtr CreateFrameBuffer(const RHIFrameBufferDesc& Desc) = 0;
	virtual RHIGraphicsPipelinePtr CreateGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc) = 0;
	virtual RHIPipelineStatePtr CreatePipelineState(const RHIGraphicsPipelineDesc& Desc) = 0;
	virtual RHIBufferPtr CreateBuffer(const RHIBufferDesc& Desc) = 0;
	virtual RHISamplerPtr CreateSampler(const RHISamplerDesc& Desc) = 0;
	virtual RHISwapchainPtr CreateSwapchain(const RHISwapchainDesc& Desc) = 0;

	virtual RHICommandListContext* GetImmediateCommandListContext(ERHIDeviceQueue Queue) = 0;
	virtual RHICommandListContextPtr AcquireDeferredCommandListContext() = 0;
	virtual void ReleaseDeferredCommandListContext(RHICommandListContextPtr& CmdListContext) = 0;

	const RHIDeviceCapabilities& GetCapabilities() const { return m_Capabilities; }

	RHIGraphicsPipeline* GetOrCreateGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc);
	RHIFrameBuffer* GetOrCreateFrameBuffer(const RHIFrameBufferDesc& Desc);
protected:
	virtual void SetupCapabilities() = 0;

	std::mutex m_CmdListContextLock;
	RHIDeviceCapabilities m_Capabilities;
private:
	std::unordered_map<size_t, RHIGraphicsPipelinePtr> m_GraphicsPipelineCache;
	std::unordered_map<size_t, RHIFrameBufferPtr> m_FrameBufferCache;
};
