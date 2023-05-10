#pragma once

#include "Colorful/IRenderer/ICommandBuffer.h"
#include "Colorful/IRenderer/IPipeline.h"

NAMESPACE_START(RHI)

#define USE_DEDICATE_TRANSFER_QUEUE 0

enum class EQueueType
{
	Graphics,
	Transfer,
	Compute
};

class IDevice
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
		}

		return "Unknown";
	}

	template<class IDType>
	static bool8_t IsPreferDedicatedDevice(IDType VendorID)
	{
		return 
			static_cast<EVendorID>(VendorID) == EVendorID::AMD ||
			static_cast<EVendorID>(VendorID) == EVendorID::NVIDIA ||
			static_cast<EVendorID>(VendorID) == EVendorID::Intel;
	}

	virtual void WaitIdle() = 0;

	virtual IShaderSharedPtr CreateShader(const ShaderDesc& Desc) = 0;

	virtual IImageSharedPtr CreateImage(const ImageDesc& Desc) = 0;

	virtual IInputLayoutSharedPtr CreateInputLayout(const InputLayoutDesc& Desc, const ShaderDesc& VertexShaderDesc) = 0;

	virtual IFrameBufferSharedPtr CreateFrameBuffer(const FrameBufferDesc& Desc) = 0;

	virtual IPipelineSharedPtr CreateGraphicsPipeline(const GraphicsPipelineDesc& Desc) = 0;

	virtual IBufferSharedPtr CreateBuffer(const BufferDesc& Desc) = 0;

	virtual ISamplerSharedPtr CreateSampler(const SamplerDesc& Desc) = 0;

	virtual ICommandBufferPoolSharedPtr CreateCommandBufferPool(EQueueType QueueType) = 0;

	virtual void SubmitCommandBuffers(EQueueType QueueType, const std::vector<ICommandBufferSharedPtr>& Commands) = 0;

	virtual void SubmitCommandBuffer(EQueueType QueueType, ICommandBufferSharedPtr& Command) = 0;

	virtual void SubmitCommandBuffers(const std::vector<ICommandBufferSharedPtr>& Commands) = 0;

	virtual void SubmitCommandBuffer(ICommandBufferSharedPtr& Command) = 0;

	virtual ICommandBufferSharedPtr GetOrAllocateCommandBuffer(EQueueType QueueType, ECommandBufferLevel Level = ECommandBufferLevel::Primary, bool8_t AutoBegin = true, bool8_t UseForTransfer = false) = 0;

	IPipelineSharedPtr GetOrCreateGraphicsPipeline(const GraphicsPipelineDesc& Desc);

	IInputLayoutSharedPtr GetOrCreateInputLayout(const InputLayoutDesc& Desc, const ShaderDesc& VertexShaderDesc);

	ISamplerSharedPtr GetOrCreateSampler(const SamplerDesc& Desc);

	IFrameBufferSharedPtr GetOrCreateFrameBuffer(const FrameBufferDesc& Desc);

	IImageSharedPtr CreateRenderTarget(
		uint32_t Width,
		uint32_t Height,
		EFormat Format,
		ESampleCount SampleCount = ESampleCount::Sample_1_Bit,
		const char8_t* Name = "NamelessRenderTarget",
		bool8_t ShaderResource = false)
	{
		assert(FormatAttribute::IsColor(Format));

		ImageDesc Desc
		{
			Width,
			Height,
			1u,
			1u,
			1u,
			EImageType::T_2D,
			Format,
			SampleCount,
			ShaderResource ? EBufferUsageFlags::RenderTarget | EBufferUsageFlags::ShaderResource : EBufferUsageFlags::RenderTarget,
			EResourceState::RenderTarget,
			Name
		};

		return CreateImage(Desc);
	}

	IImageSharedPtr CreateDepthStencil(uint32_t Width, uint32_t Height, EFormat Format, const char8_t* Name = "NamelessDepthStencil")
	{
		assert(FormatAttribute::IsDepthStenci(Format));

		ImageDesc Desc
		{
			Width,
			Height,
			1u,
			1u,
			1u,
			EImageType::T_2D,
			Format,
			ESampleCount::Sample_1_Bit,
			EBufferUsageFlags::DepthStencil,
			EResourceState::DepthWrite,
			Name
		};

		return CreateImage(Desc);
	}

	const char8_t* const AdapterName() const
	{
		return m_Adapter.c_str();
	}

	IBufferSharedPtr CreateUniformBuffer(size_t Size)
	{
		return CreateBuffer(
			BufferDesc
			{
				EBufferUsageFlags::UniformBuffer,
				EDeviceAccessFlags::GpuReadCpuWrite,
				EResourceState::UniformBuffer,
				Size
			}
		);
	}

	IBufferSharedPtr CreateVertexBuffer(size_t Size, EDeviceAccessFlags AccessFlags, const void* InitialData)
	{
		return CreateBuffer(
			BufferDesc
			{
				EBufferUsageFlags::VertexBuffer,
				AccessFlags,
				EResourceState::VertexBuffer,
				Size,
				InitialData
			}
		);
	}

	IBufferSharedPtr CreateIndexBuffer(size_t Size, EDeviceAccessFlags AccessFlags, const void* InitialData)
	{
		return CreateBuffer(
			BufferDesc
			{
				EBufferUsageFlags::IndexBuffer,
				AccessFlags,
				EResourceState::IndexBuffer,
				Size,
				InitialData
			}
		);
	}
protected:
	std::string m_Adapter;
protected:
	std::pair<std::mutex, std::unordered_map<size_t, ISamplerSharedPtr>> m_Samplers;
	std::pair<std::mutex, std::unordered_map<size_t, IInputLayoutSharedPtr>> m_InputLayouts;
	std::unordered_map<size_t, IFrameBufferSharedPtr> m_FrameBufferCache;
	std::unordered_map<size_t, IPipelineSharedPtr> m_GraphicsPipelineCache;
};

class ISwapchain
{
public:
	ISwapchain() = default;

	ISwapchain(uint64_t WindowHandle, uint32_t Width, uint32_t Height, bool8_t Fullscreen, bool8_t VSync)
		: m_WindowHandle(WindowHandle)
		, m_Width(Width)
		, m_Height(Height)
		, m_Fullscreen(Fullscreen)
		, m_VSync(VSync)
	{
		assert(m_WindowHandle);
	}

	virtual ~ISwapchain() = default;

	inline uint32_t Width() const
	{
		return m_Width;
	}

	inline uint32_t Height() const
	{
		return m_Height;
	}

	inline bool8_t IsFullscreen() const
	{
		return m_Fullscreen;
	}

	inline bool8_t IsVSyncOn() const
	{
		return m_VSync;
	}
protected:
	bool8_t m_VSync = false;
	bool8_t m_Fullscreen = false;
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;

	const uint64_t m_WindowHandle;
private:
};

NAMESPACE_END(RHI)
