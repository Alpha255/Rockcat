#pragma once

#include "Runtime/Engine/RHI/RHICommandBuffer.h"

enum class ERHIDeviceQueueType : uint8_t
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
	static const char8_t* const GetVendorName(IDType VendorID)
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
	static bool8_t IsDedicatedDevice(IDType VendorID)
	{
		return 
			static_cast<EVendorID>(VendorID) == EVendorID::AMD ||
			static_cast<EVendorID>(VendorID) == EVendorID::NVIDIA ||
			static_cast<EVendorID>(VendorID) == EVendorID::Intel;
	}

	virtual void WaitIdle() = 0;

	virtual RHIShaderPtr CreateShader(const RHIShaderCreateInfo& RHICreateInfo) = 0;

	virtual RHIImagePtr CreateImage(const RHIImageCreateInfo& RHICreateInfo) = 0;

	virtual RHIInputLayoutPtr CreateInputLayout(const RHIInputLayoutCreateInfo& RHICreateInfo) = 0;

	virtual RHIFrameBufferPtr CreateFrameBuffer(const RHIFrameBufferCreateInfo& RHICreateInfo) = 0;

	virtual RHIGraphicsPipelinePtr CreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& RHICreateInfo) = 0;

	virtual RHIBufferPtr CreateBuffer(const RHIBufferCreateInfo& RHICreateInfo) = 0;

	virtual RHISamplerPtr CreateSampler(const RHISamplerCreateInfo& RHICreateInfo) = 0;

	virtual RHICommandBufferPoolPtr CreateCommandBufferPool(ERHIDeviceQueueType QueueType) = 0;

	virtual void SubmitCommandBuffers(ERHIDeviceQueueType QueueType, const std::vector<RHICommandBuffer*>& Commands) = 0;

	virtual void SubmitCommandBuffer(ERHIDeviceQueueType QueueType, RHICommandBuffer* Command) = 0;

	virtual void SubmitCommandBuffers(const std::vector<RHICommandBuffer*>& Commands) = 0;

	virtual void SubmitCommandBuffer(RHICommandBuffer* Command) = 0;

	virtual RHICommandBufferPtr GetOrAllocateCommandBuffer(ERHIDeviceQueueType QueueType, ERHICommandBufferLevel Level = ERHICommandBufferLevel::Primary, bool8_t AutoBegin = true) = 0;

	RHIImagePtr CreateColorAttachment(
		uint32_t Width,
		uint32_t Height,
		ERHIFormat Format,
		ERHISampleCount SampleCount = ERHISampleCount::Sample_1_Bit,
		const char8_t* Name = "NamelessRenderTarget",
		bool8_t UseForShaderResource = false)
	{
		assert(RHI::IsColor(Format));

		RHIImageCreateInfo RHICreateInfo
		{
			Width,
			Height,
			1u,
			1u,
			1u,
			ERHIImageType::T_2D,
			Format,
			SampleCount,
			UseForShaderResource ? ERHIBufferUsageFlags::RenderTarget | ERHIBufferUsageFlags::ShaderResource : ERHIBufferUsageFlags::RenderTarget,
			ERHIResourceState::RenderTarget,
			Name
		};
		return CreateImage(RHICreateInfo);
	}

	RHIImagePtr CreateDepthStencil(uint32_t Width, uint32_t Height, ERHIFormat Format, const char8_t* Name = "NamelessDepthStencil")
	{
		assert(RHI::IsDepthStenci(Format));

		RHIImageCreateInfo RHICreateInfo
		{
			Width,
			Height,
			1u,
			1u,
			1u,
			ERHIImageType::T_2D,
			Format,
			ERHISampleCount::Sample_1_Bit,
			ERHIBufferUsageFlags::DepthStencil,
			ERHIResourceState::DepthWrite,
			Name
		};
		return CreateImage(RHICreateInfo);
	}

	RHIBufferPtr CreateUniformBuffer(size_t Size)
	{
		return CreateBuffer(
			RHIBufferCreateInfo
			{
				ERHIBufferUsageFlags::UniformBuffer,
				ERHIDeviceAccessFlags::GpuReadCpuWrite,
				ERHIResourceState::UniformBuffer,
				Size
			}
		);
	}

	RHIBufferPtr CreateVertexBuffer(size_t Size, ERHIDeviceAccessFlags AccessFlags, const void* InitialData)
	{
		return CreateBuffer(
			RHIBufferCreateInfo
			{
				ERHIBufferUsageFlags::VertexBuffer,
				AccessFlags,
				ERHIResourceState::VertexBuffer,
				Size,
				InitialData
			}
		);
	}

	RHIBufferPtr CreateIndexBuffer(size_t Size, ERHIDeviceAccessFlags AccessFlags, const void* InitialData)
	{
		return CreateBuffer(
			RHIBufferCreateInfo
			{
				ERHIBufferUsageFlags::IndexBuffer,
				AccessFlags,
				ERHIResourceState::IndexBuffer,
				Size,
				InitialData
			}
		);
	}

	const char8_t* const GetAdapterName() const { return m_AdapterName.c_str(); }
private:
	std::string m_AdapterName;
};

class RHISwapchain : public RHIResource
{
public:
	using RHIResource::RHIResource;

	RHISwapchain(const void* WindowHandle, uint32_t Width, uint32_t Height, bool8_t Fullscreen, bool8_t VSync)
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
	inline bool8_t IsFullscreen() const { return m_Fullscreen; }
	inline bool8_t IsEnableVSync() const { return m_VSync; }
private:
	bool8_t m_VSync = false;
	bool8_t m_Fullscreen = false;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;

	const void* m_WindowHandle = nullptr;
};
