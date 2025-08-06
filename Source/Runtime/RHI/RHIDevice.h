#pragma once

#include "RHI/RHICommandBuffer.h"
#include "Services/SpdLogService.h"

enum class ERHIDeviceQueue : uint8_t
{
	Graphics,
	Transfer,
	Compute,
	Num
};

struct RHIDeviceCaps
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
	uint32_t MaxSamplers = 0u;
	uint32_t MaxDescriptorSets = 0u;
	float MaxSamplerAnisotropy = 0.0f;
};

class RHIDevice
{
public:
	enum class EVendorID
	{
		AMD = 0x1002,
		ImgTec = 0x1010,
		NVIDIA = 0x10DE,
		ARM = 0x13B5,
		Qualcomm = 0x5143,
		Intel = 0x8086,
		MiscrosoftWrap = 0x1414
	};

	template<class IDType>
	static const char* const GetVendorName(IDType VendorID)
	{
		switch (static_cast<EVendorID>(VendorID))
		{
		case EVendorID::AMD: return "AMD";
		case EVendorID::ImgTec: return "ImgTec";
		case EVendorID::NVIDIA: return "NVIDIA";
		case EVendorID::ARM: return "ARM";
		case EVendorID::Qualcomm: return "Qualcomm";
		case EVendorID::Intel: return "Intel";
		case EVendorID::MiscrosoftWrap: return "Miscrosoft Wrap";
		default: return "Unknown";
		}
	}

	template<class IDType>
	static bool IsDedicatedDevice(IDType VendorID)
	{
		return 
			static_cast<EVendorID>(VendorID) == EVendorID::AMD ||
			static_cast<EVendorID>(VendorID) == EVendorID::NVIDIA ||
			static_cast<EVendorID>(VendorID) == EVendorID::Intel;
	}

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

	RHIGraphicsPipeline* GetOrCreateGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc);
	RHIFrameBuffer* GetOrCreateFrameBuffer(const RHIFrameBufferDesc& Desc);

	const char* const GetAdapterName() const { return m_AdapterName.c_str(); }

	inline bool SupportsAsyncCompute() const { return m_SupportsAsyncCompute; }
	inline bool SupportsTransferQueue() const { return m_SupportsTransferQueue; }
protected:
	std::mutex m_CmdListContextLock;

	inline void SetSupportsAsyncCompute(bool Value) { m_SupportsAsyncCompute = Value; }
	inline void SetSupportsTransferQueue(bool Value) { m_SupportsTransferQueue = Value; }
private:
	std::string m_AdapterName;

	std::unordered_map<size_t, RHIGraphicsPipelinePtr> m_GraphicsPipelineCache;
	std::unordered_map<size_t, RHIFrameBufferPtr> m_FrameBufferCache;

	bool m_SupportsAsyncCompute = false;
	bool m_SupportsTransferQueue = false;
};
