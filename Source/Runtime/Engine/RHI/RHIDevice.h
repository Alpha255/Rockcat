#pragma once

#include "Engine/RHI/RHICommandBuffer.h"

enum class ERHIDeviceQueue : uint8_t
{
	Graphics,
	Transfer,
	Compute,
	Num
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

	virtual RHIShaderPtr CreateShader(const RHIShaderCreateInfo& RHICreateInfo) = 0;
	virtual RHITexturePtr CreateTexture(const RHITextureCreateInfo& RHICreateInfo) = 0;
	virtual RHIInputLayoutPtr CreateInputLayout(const RHIInputLayoutCreateInfo& RHICreateInfo) = 0;
	virtual RHIFrameBufferPtr CreateFrameBuffer(const RHIFrameBufferCreateInfo& RHICreateInfo) = 0;
	virtual RHIGraphicsPipelinePtr CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& RHICreateInfo) = 0;
	virtual RHIPipelineStatePtr CreatePipelineState(const RHIGraphicsPipelineCreateInfo& RHICreateInfo) = 0;
	virtual RHIBufferPtr CreateBuffer(const RHIBufferCreateInfo& RHICreateInfo) = 0;
	virtual RHISamplerPtr CreateSampler(const RHISamplerCreateInfo& RHICreateInfo) = 0;

	virtual RHICommandListContext* GetImmediateCommandListContext(ERHIDeviceQueue Queue) = 0;
	virtual RHICommandListContextPtr AcquireDeferredCommandListContext() = 0;
	virtual void ReleaseDeferredCommandListContext(RHICommandListContextPtr& CmdListContext) = 0;

	const char* const GetAdapterName() const { return m_AdapterName.c_str(); }
protected:
	std::mutex m_CmdListContextLock;
private:
	std::string m_AdapterName;
};

class RHISwapchain
{
public:
	RHISwapchain(const void* WindowHandle, uint32_t Width, uint32_t Height, bool Fullscreen, bool VSync)
		: m_WindowHandle(WindowHandle)
		, m_Width(Width)
		, m_Height(Height)
		, m_Fullscreen(Fullscreen)
		, m_VSync(VSync)
	{
		assert(m_WindowHandle);
	}

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t Height() const { return m_Height; }
	inline bool IsFullscreen() const { return m_Fullscreen; }
	inline bool IsEnableVSync() const { return m_VSync; }
private:
	bool m_VSync = false;
	bool m_Fullscreen = false;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;

	const void* m_WindowHandle = nullptr;
};
