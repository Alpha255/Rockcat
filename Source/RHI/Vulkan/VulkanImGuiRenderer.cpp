#include "RHI/Vulkan/VulkanImGuiRenderer.h"

#include "RHI/Vulkan/VulkanBuffer.h"
#include "RHI/Vulkan/VulkanCommandBuffer.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanTexture.h"
#include "Misc/Paths.h"

#include <imgui/imgui.h>

#include <fstream>

namespace
{
	/// Extra vertices/indices to grow the buffers by, so a slowly growing UI does not rebuild them
	/// on every frame.
	static constexpr size_t kBufferGrowSlack = 5000u;

	std::vector<uint32_t> LoadSpirv(const std::filesystem::path& Path)
	{
		std::ifstream File(Path, std::ios::ate | std::ios::binary);
		if (!File.is_open())
		{
			LOG_ERROR(LogVulkan, "ImGui: Failed to open shader binary \"{}\"", Path.string());
			return {};
		}

		const auto FileSize = static_cast<size_t>(File.tellg());
		if (FileSize == 0u || (FileSize % sizeof(uint32_t)) != 0u)
		{
			LOG_ERROR(LogVulkan, "ImGui: \"{}\" is not a valid SPIR-V binary ({} bytes)", Path.string(), FileSize);
			return {};
		}

		std::vector<uint32_t> Spirv(FileSize / sizeof(uint32_t));
		File.seekg(0, std::ios::beg);
		File.read(reinterpret_cast<char*>(Spirv.data()), static_cast<std::streamsize>(FileSize));

		return Spirv;
	}
}

VulkanImGuiRenderer::~VulkanImGuiRenderer()
{
	Shutdown();
}

const vk::Device& VulkanImGuiRenderer::GetNativeDevice() const
{
	return m_Device->GetNative();
}

bool VulkanImGuiRenderer::Init(const RHIDevice& Device, vk::Format ColorFormat)
{
	assert(ColorFormat != vk::Format::eUndefined);

	m_Device = &static_cast<const VulkanDevice&>(Device);
	m_ColorFormat = ColorFormat;

	if (!CreateShaderModules())
	{
		Shutdown();
		return false;
	}

	CreateLayouts();
	CreateRenderPass();

	if (!CreateFontResources(Device) || !CreatePipeline())
	{
		Shutdown();
		return false;
	}

	return true;
}

void VulkanImGuiRenderer::Shutdown()
{
	const vk::Device& NativeDevice = m_Device ? m_Device->GetNative() : vk::Device();

	if (NativeDevice)
	{
		for (auto& [Texture, Target] : m_BackBufferTargets)
		{
			NativeDevice.destroy(Target.FrameBuffer, VK_ALLOCATION_CALLBACKS);
			NativeDevice.destroy(Target.View, VK_ALLOCATION_CALLBACKS);
		}

		if (m_Pipeline)
		{
			NativeDevice.destroy(m_Pipeline, VK_ALLOCATION_CALLBACKS);
		}
		if (m_RenderPass)
		{
			NativeDevice.destroy(m_RenderPass, VK_ALLOCATION_CALLBACKS);
		}
		if (m_DescriptorPool)
		{
			NativeDevice.destroy(m_DescriptorPool, VK_ALLOCATION_CALLBACKS);
		}
		if (m_FontImageView)
		{
			NativeDevice.destroy(m_FontImageView, VK_ALLOCATION_CALLBACKS);
		}
		if (m_PipelineLayout)
		{
			NativeDevice.destroy(m_PipelineLayout, VK_ALLOCATION_CALLBACKS);
		}
		if (m_DescriptorSetLayout)
		{
			NativeDevice.destroy(m_DescriptorSetLayout, VK_ALLOCATION_CALLBACKS);
		}
		if (m_FragmentShaderModule)
		{
			NativeDevice.destroy(m_FragmentShaderModule, VK_ALLOCATION_CALLBACKS);
		}
		if (m_VertexShaderModule)
		{
			NativeDevice.destroy(m_VertexShaderModule, VK_ALLOCATION_CALLBACKS);
		}
	}

	m_BackBufferTargets.clear();

	m_Pipeline = nullptr;
	m_RenderPass = nullptr;
	m_DescriptorPool = nullptr;
	m_DescriptorSet = nullptr;
	m_FontImageView = nullptr;
	m_PipelineLayout = nullptr;
	m_DescriptorSetLayout = nullptr;
	m_FragmentShaderModule = nullptr;
	m_VertexShaderModule = nullptr;

	m_FontSampler.reset();
	m_FontTexture.reset();
	m_VertexBuffer.reset();
	m_IndexBuffer.reset();

	m_VertexBufferBytes = 0ull;
	m_IndexBufferBytes = 0ull;
	m_ColorFormat = vk::Format::eUndefined;
	m_Device = nullptr;
}

bool VulkanImGuiRenderer::CreateShaderModules()
{
	/// Built from Assets/Shaders/ImGui.{vert,frag} with:
	///   dxc -T vs_6_0 -E main -spirv -D_SPIRV_ -I <repo>/Assets Assets/Shaders/ImGui.vert -Fo ImGui.vert.spv
	/// The compilation service in ShaderLibrary.cpp is disabled, so for now the binaries are
	/// produced offline and loaded from Paths::ShaderBinaryPath().
	const std::vector<uint32_t> VertexSpirv = LoadSpirv(Paths::ShaderBinaryPath() / "ImGui.vert.spv");
	const std::vector<uint32_t> FragmentSpirv = LoadSpirv(Paths::ShaderBinaryPath() / "ImGui.frag.spv");

	if (VertexSpirv.empty() || FragmentSpirv.empty())
	{
		return false;
	}

	vk::ShaderModuleCreateInfo VertexCreateInfo;
	VertexCreateInfo.setCodeSize(VertexSpirv.size() * sizeof(uint32_t))
		.setPCode(VertexSpirv.data());
	VERIFY_VK(GetNativeDevice().createShaderModule(&VertexCreateInfo, VK_ALLOCATION_CALLBACKS, &m_VertexShaderModule));

	vk::ShaderModuleCreateInfo FragmentCreateInfo;
	FragmentCreateInfo.setCodeSize(FragmentSpirv.size() * sizeof(uint32_t))
		.setPCode(FragmentSpirv.data());
	VERIFY_VK(GetNativeDevice().createShaderModule(&FragmentCreateInfo, VK_ALLOCATION_CALLBACKS, &m_FragmentShaderModule));

	return true;
}

void VulkanImGuiRenderer::CreateLayouts()
{
	/// ImGui.frag declares FontImage (register t0) and LinearSampler (register s0) as two separate
	/// resources, which SPIR-V turns into two separate bindings in set 0.
	const vk::DescriptorSetLayoutBinding Bindings[]
	{
		vk::DescriptorSetLayoutBinding(0u, vk::DescriptorType::eSampledImage, 1u, vk::ShaderStageFlagBits::eFragment),
		vk::DescriptorSetLayoutBinding(1u, vk::DescriptorType::eSampler, 1u, vk::ShaderStageFlagBits::eFragment)
	};

	vk::DescriptorSetLayoutCreateInfo SetLayoutCreateInfo;
	SetLayoutCreateInfo.setBindingCount(static_cast<uint32_t>(std::size(Bindings)))
		.setPBindings(Bindings);
	VERIFY_VK(GetNativeDevice().createDescriptorSetLayout(&SetLayoutCreateInfo, VK_ALLOCATION_CALLBACKS, &m_DescriptorSetLayout));

	/// ImGui.vert reads a single float2 InverseDisplaySize out of the push constants.
	const vk::PushConstantRange PushConstantRange(vk::ShaderStageFlagBits::eVertex, 0u, sizeof(float) * 2u);

	vk::PipelineLayoutCreateInfo LayoutCreateInfo;
	LayoutCreateInfo.setSetLayoutCount(1u)
		.setPSetLayouts(&m_DescriptorSetLayout)
		.setPushConstantRangeCount(1u)
		.setPPushConstantRanges(&PushConstantRange);
	VERIFY_VK(GetNativeDevice().createPipelineLayout(&LayoutCreateInfo, VK_ALLOCATION_CALLBACKS, &m_PipelineLayout));
}

void VulkanImGuiRenderer::CreateRenderPass()
{
	/// The UI is drawn over the colour that ClearColorTexture() already wrote, so the attachment is
	/// loaded rather than cleared, and both layouts are COLOR_ATTACHMENT_OPTIMAL to match what
	/// ClearColorTexture() leaves behind and TransitionToPresent() expects.
	vk::AttachmentDescription ColorAttachment;
	ColorAttachment.setFormat(m_ColorFormat)
		.setSamples(vk::SampleCountFlagBits::e1)
		.setLoadOp(vk::AttachmentLoadOp::eLoad)
		.setStoreOp(vk::AttachmentStoreOp::eStore)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setInitialLayout(vk::ImageLayout::eColorAttachmentOptimal)
		.setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal);

	const vk::AttachmentReference ColorReference(0u, vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription Subpass;
	Subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachmentCount(1u)
		.setPColorAttachments(&ColorReference);

	/// Wait for the clear (a transfer) recorded earlier in the same command buffer to land before
	/// the pass starts writing colours.
	vk::SubpassDependency Dependency;
	Dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL)
		.setDstSubpass(0u)
		.setSrcStageMask(vk::PipelineStageFlagBits::eTransfer | vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setSrcAccessMask(vk::AccessFlagBits::eTransferWrite | vk::AccessFlagBits::eColorAttachmentWrite)
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

	vk::RenderPassCreateInfo RenderPassCreateInfo;
	RenderPassCreateInfo.setAttachmentCount(1u)
		.setPAttachments(&ColorAttachment)
		.setSubpassCount(1u)
		.setPSubpasses(&Subpass)
		.setDependencyCount(1u)
		.setPDependencies(&Dependency);

	VERIFY_VK(GetNativeDevice().createRenderPass(&RenderPassCreateInfo, VK_ALLOCATION_CALLBACKS, &m_RenderPass));
}

bool VulkanImGuiRenderer::CreateFontResources(const RHIDevice& Device)
{
	ImGuiIO& IO = ImGui::GetIO();
	assert(IO.Fonts);

	unsigned char* Pixels = nullptr;
	int Width = 0;
	int Height = 0;
	IO.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);

	if (!Pixels || Width <= 0 || Height <= 0)
	{
		LOG_ERROR(LogVulkan, "ImGui: The font atlas produced no texture data.");
		return false;
	}

	/// The pixels stay owned by ImFontAtlas. RHIUploadManager streams them into device local memory
	/// and lands the texture in ERHIResourceState::ShaderResource, so no copy is needed here.
	RHITextureDesc TextureDesc;
	TextureDesc.SetWidth(static_cast<uint32_t>(Width))
		.SetHeight(static_cast<uint32_t>(Height))
		.SetDepth(1u)
		.SetNumArrayLayer(1u)
		.SetNumMipLevel(1u)
		.SetFormat(ERHIFormat::RGBA8_UNorm)
		.SetDimension(ERHITextureDimension::T_2D)
		.SetSampleCount(ERHISampleCount::Sample_1_Bit)
		.SetUsages(ERHIBufferUsageFlags::ShaderResource)
		.SetPermanentState(ERHIResourceState::ShaderResource)
		.SetBulkData(static_cast<size_t>(Width) * static_cast<size_t>(Height) * 4u, Pixels)
		.SetName("ImGuiFontAtlas");

	m_FontTexture = Device.CreateTexture(TextureDesc);

	if (!m_FontTexture)
	{
		LOG_ERROR(LogVulkan, "ImGui: Failed to create the font atlas texture.");
		return false;
	}

	/// CompareOp has to be Always: VulkanSampler derives compareEnable from it, and comparison is
	/// only meaningful for depth textures.
	RHISamplerDesc SamplerDesc;
	SamplerDesc.SetMinMagFilter(ERHIFilter::Linear)
		.SetMipmapMode(ERHIFilter::Linear)
		.SetAddressModeU(ERHISamplerAddressMode::ClampToEdge)
		.SetAddressModeV(ERHISamplerAddressMode::ClampToEdge)
		.SetAddressModeW(ERHISamplerAddressMode::ClampToEdge)
		.SetCompareOp(ERHICompareFunc::Always)
		.SetName("ImGuiFontSampler");

	m_FontSampler = Device.CreateSampler(SamplerDesc);

	if (!m_FontSampler)
	{
		LOG_ERROR(LogVulkan, "ImGui: Failed to create the font atlas sampler.");
		return false;
	}

	vk::ImageViewCreateInfo ViewCreateInfo;
	ViewCreateInfo.setImage(static_cast<const VulkanTexture*>(m_FontTexture.get())->GetNative())
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(vk::Format::eR8G8B8A8Unorm)
		.setComponents(vk::ComponentMapping())
		.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u));
	VERIFY_VK(GetNativeDevice().createImageView(&ViewCreateInfo, VK_ALLOCATION_CALLBACKS, &m_FontImageView));

	/// The font atlas is the only resource the UI samples, so a single set is all that is needed.
	const vk::DescriptorPoolSize PoolSizes[]
	{
		vk::DescriptorPoolSize(vk::DescriptorType::eSampledImage, 1u),
		vk::DescriptorPoolSize(vk::DescriptorType::eSampler, 1u)
	};

	vk::DescriptorPoolCreateInfo PoolCreateInfo;
	PoolCreateInfo.setMaxSets(1u)
		.setPoolSizeCount(static_cast<uint32_t>(std::size(PoolSizes)))
		.setPPoolSizes(PoolSizes);
	VERIFY_VK(GetNativeDevice().createDescriptorPool(&PoolCreateInfo, VK_ALLOCATION_CALLBACKS, &m_DescriptorPool));

	vk::DescriptorSetAllocateInfo AllocateInfo;
	AllocateInfo.setDescriptorPool(m_DescriptorPool)
		.setDescriptorSetCount(1u)
		.setPSetLayouts(&m_DescriptorSetLayout);
	VERIFY_VK(GetNativeDevice().allocateDescriptorSets(&AllocateInfo, &m_DescriptorSet));

	/// The image layout has to match the state the upload left the texture in:
	/// ERHIResourceState::ShaderResource -> VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.
	const vk::DescriptorImageInfo ImageInfo(
		static_cast<const VulkanSampler*>(m_FontSampler.get())->GetNative(),
		m_FontImageView,
		vk::ImageLayout::eShaderReadOnlyOptimal);

	const vk::WriteDescriptorSet Writes[]
	{
		vk::WriteDescriptorSet(m_DescriptorSet, 0u, 0u, 1u, vk::DescriptorType::eSampledImage, &ImageInfo, nullptr, nullptr),
		vk::WriteDescriptorSet(m_DescriptorSet, 1u, 0u, 1u, vk::DescriptorType::eSampler, &ImageInfo, nullptr, nullptr)
	};

	GetNativeDevice().updateDescriptorSets(static_cast<uint32_t>(std::size(Writes)), Writes, 0u, nullptr);

	return true;
}

void VulkanImGuiRenderer::DestroyFontResources()
{
	if (m_Device)
	{
		const vk::Device& NativeDevice = GetNativeDevice();

		/// The descriptor set belongs to the pool, so throwing the pool away already frees it.
		if (m_DescriptorPool)
		{
			NativeDevice.destroy(m_DescriptorPool, VK_ALLOCATION_CALLBACKS);
			m_DescriptorPool = nullptr;
		}
		if (m_FontImageView)
		{
			NativeDevice.destroy(m_FontImageView, VK_ALLOCATION_CALLBACKS);
			m_FontImageView = nullptr;
		}
	}

	m_DescriptorSet = nullptr;
	m_FontSampler.reset();
	m_FontTexture.reset();
}

bool VulkanImGuiRenderer::CreatePipeline()
{
	/// Matches ImDrawVert: pos (float2), uv (float2), col (RGBA8 unorm).
	const vk::VertexInputBindingDescription VertexBinding(0u, sizeof(ImDrawVert), vk::VertexInputRate::eVertex);
	const vk::VertexInputAttributeDescription VertexAttributes[]
	{
		vk::VertexInputAttributeDescription(0u, 0u, vk::Format::eR32G32Sfloat, offsetof(ImDrawVert, pos)),
		vk::VertexInputAttributeDescription(1u, 0u, vk::Format::eR32G32Sfloat, offsetof(ImDrawVert, uv)),
		vk::VertexInputAttributeDescription(2u, 0u, vk::Format::eR8G8B8A8Unorm, offsetof(ImDrawVert, col))
	};

	vk::PipelineVertexInputStateCreateInfo VertexInputState;
	VertexInputState.setVertexBindingDescriptionCount(1u)
		.setPVertexBindingDescriptions(&VertexBinding)
		.setVertexAttributeDescriptionCount(static_cast<uint32_t>(std::size(VertexAttributes)))
		.setPVertexAttributeDescriptions(VertexAttributes);

	vk::PipelineInputAssemblyStateCreateInfo InputAssemblyState;
	InputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList)
		.setPrimitiveRestartEnable(false);

	vk::PipelineTessellationStateCreateInfo TessellationState;

	vk::PipelineViewportStateCreateInfo ViewportState;
	ViewportState.setViewportCount(1u)
		.setScissorCount(1u);

	vk::PipelineRasterizationStateCreateInfo RasterizationState;
	RasterizationState.setPolygonMode(vk::PolygonMode::eFill)
		.setCullMode(vk::CullModeFlagBits::eNone)
		.setFrontFace(vk::FrontFace::eCounterClockwise)
		.setLineWidth(1.0f);

	vk::PipelineMultisampleStateCreateInfo MultisampleState;
	MultisampleState.setRasterizationSamples(vk::SampleCountFlagBits::e1);

	vk::PipelineDepthStencilStateCreateInfo DepthStencilState;
	DepthStencilState.setDepthTestEnable(false)
		.setDepthWriteEnable(false)
		.setStencilTestEnable(false);

	/// Standard ImGui blending: the atlas is a coverage map, so colour is a normal over blend while
	/// the destination alpha is replaced by the source coverage.
	vk::PipelineColorBlendAttachmentState BlendAttachment;
	BlendAttachment.setBlendEnable(true)
		.setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
		.setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
		.setColorBlendOp(vk::BlendOp::eAdd)
		.setSrcAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
		.setDstAlphaBlendFactor(vk::BlendFactor::eZero)
		.setAlphaBlendOp(vk::BlendOp::eAdd)
		.setColorWriteMask(
			vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
			vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

	vk::PipelineColorBlendStateCreateInfo BlendState;
	BlendState.setAttachmentCount(1u)
		.setPAttachments(&BlendAttachment);

	/// The viewport and the scissor rect change per frame (window resize) and per draw command.
	const vk::DynamicState DynamicStates[]
	{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo DynamicState;
	DynamicState.setDynamicStateCount(static_cast<uint32_t>(std::size(DynamicStates)))
		.setPDynamicStates(DynamicStates);

	const vk::PipelineShaderStageCreateInfo ShaderStages[]
	{
		vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, m_VertexShaderModule, "main"),
		vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, m_FragmentShaderModule, "main")
	};

	vk::GraphicsPipelineCreateInfo PipelineCreateInfo;
	PipelineCreateInfo.setStages(ShaderStages)
		.setPVertexInputState(&VertexInputState)
		.setPInputAssemblyState(&InputAssemblyState)
		.setPTessellationState(&TessellationState)
		.setPViewportState(&ViewportState)
		.setPRasterizationState(&RasterizationState)
		.setPMultisampleState(&MultisampleState)
		.setPDepthStencilState(&DepthStencilState)
		.setPColorBlendState(&BlendState)
		.setPDynamicState(&DynamicState)
		.setLayout(m_PipelineLayout)
		.setRenderPass(m_RenderPass);

	VERIFY_VK(GetNativeDevice().createGraphicsPipelines(nullptr, 1u, &PipelineCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Pipeline));

	return true;
}

const VulkanImGuiRenderer::BackBufferTarget& VulkanImGuiRenderer::GetOrCreateBackBufferTarget(const RHITexture* Texture)
{
	const uint32_t Width = Texture->GetWidth();
	const uint32_t Height = Texture->GetHeight();

	auto It = m_BackBufferTargets.find(Texture);
	if (It != m_BackBufferTargets.end())
	{
		if (It->second.Width == Width && It->second.Height == Height)
		{
			return It->second;
		}

		/// The back buffer changed extent underneath us, so the cached image is a different one and
		/// the view has to follow it. The Swapchain this texture came from was already rebuilt and
		/// the old image may well be gone.
		auto Cached = It->second;
		DestroyBackBufferTarget(Cached);
		m_BackBufferTargets.erase(It);
	}

	vk::ImageViewCreateInfo ViewCreateInfo;
	ViewCreateInfo.setImage(static_cast<const VulkanTexture*>(Texture)->GetNative())
		.setViewType(vk::ImageViewType::e2D)
		.setFormat(m_ColorFormat)
		.setComponents(vk::ComponentMapping())
		.setSubresourceRange(vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, 1u, 0u, 1u));

	vk::ImageView View;
	VERIFY_VK(GetNativeDevice().createImageView(&ViewCreateInfo, VK_ALLOCATION_CALLBACKS, &View));

	vk::FramebufferCreateInfo FrameBufferCreateInfo;
	FrameBufferCreateInfo.setRenderPass(m_RenderPass)
		.setAttachmentCount(1u)
		.setPAttachments(&View)
		.setWidth(Width)
		.setHeight(Height)
		.setLayers(1u);

	vk::Framebuffer FrameBuffer;
	VERIFY_VK(GetNativeDevice().createFramebuffer(&FrameBufferCreateInfo, VK_ALLOCATION_CALLBACKS, &FrameBuffer));

	return m_BackBufferTargets.emplace(Texture, BackBufferTarget{ View, FrameBuffer, Width, Height }).first->second;
}

void VulkanImGuiRenderer::DestroyBackBufferTarget(BackBufferTarget& Target)
{
	const vk::Device& NativeDevice = GetNativeDevice();

	if (NativeDevice)
	{
		NativeDevice.destroy(Target.FrameBuffer, VK_ALLOCATION_CALLBACKS);
		NativeDevice.destroy(Target.View, VK_ALLOCATION_CALLBACKS);
	}

	Target = BackBufferTarget();
}

void VulkanImGuiRenderer::InvalidateBackBufferCache()
{
	if (!m_Device)
	{
		return;
	}

	for (auto& [Texture, Target] : m_BackBufferTargets)
	{
		DestroyBackBufferTarget(Target);
	}

	m_BackBufferTargets.clear();
}

bool VulkanImGuiRenderer::RebuildFontAtlas(const RHIDevice& Device)
{
	if (!IsInitialized())
	{
		return false;
	}

	DestroyFontResources();

	return CreateFontResources(Device);
}

void VulkanImGuiRenderer::ResizeBuffer(
	RHIBufferPtr& Buffer, size_t& CurrentBytes, size_t RequiredBytes, ERHIBufferUsageFlags Usages, const char* DebugName)
{
	if (RequiredBytes == 0u)
	{
		return;
	}

	if (Buffer && CurrentBytes >= RequiredBytes)
	{
		return;
	}

	CurrentBytes = RequiredBytes + kBufferGrowSlack;

	RHIBufferDesc Desc;
	Desc.SetSize(CurrentBytes)
		.SetUsages(Usages)
		.SetAccessFlags(ERHIDeviceAccessFlags::GpuReadCpuWrite)
		.SetName(DebugName);

	Buffer = m_Device->CreateBuffer(Desc);
}

void VulkanImGuiRenderer::RenderDrawData(RHICommandBuffer* CommandBuffer, const RHITexture* BackBuffer)
{
	const ImDrawData* DrawData = ImGui::GetDrawData();

	if (!IsInitialized() || !CommandBuffer || !BackBuffer || !DrawData ||
		!DrawData->Valid || DrawData->CmdListsCount == 0 || DrawData->TotalVtxCount == 0)
	{
		return;
	}

	auto VulkanCommand = static_cast<VulkanCommandBuffer*>(CommandBuffer);
	assert(VulkanCommand->IsRecording());

	const size_t VertexBytes = static_cast<size_t>(DrawData->TotalVtxCount) * sizeof(ImDrawVert);
	const size_t IndexBytes = static_cast<size_t>(DrawData->TotalIdxCount) * sizeof(ImDrawIdx);

	ResizeBuffer(m_VertexBuffer, m_VertexBufferBytes, VertexBytes, ERHIBufferUsageFlags::VertexBuffer, "ImGuiVertexBuffer");
	ResizeBuffer(m_IndexBuffer, m_IndexBufferBytes, IndexBytes, ERHIBufferUsageFlags::IndexBuffer, "ImGuiIndexBuffer");

	if (!m_VertexBuffer || !m_IndexBuffer)
	{
		return;
	}

	/// Map each buffer once and stream every command list into it back to back. The memory is host
	/// coherent, so no flush is needed (and FlushMappedRange() asserts on coherent memory).
	auto VertexData = static_cast<uint8_t*>(m_VertexBuffer->Map(ERHIMapMode::WriteOnly));
	auto IndexData = static_cast<uint8_t*>(m_IndexBuffer->Map(ERHIMapMode::WriteOnly));
	assert(VertexData && IndexData);

	size_t VertexWriteOffset = 0ull;
	size_t IndexWriteOffset = 0ull;
	for (int CmdListIndex = 0; CmdListIndex < DrawData->CmdListsCount; ++CmdListIndex)
	{
		const ImDrawList* DrawList = DrawData->CmdLists[CmdListIndex];

		const size_t ListVertexBytes = static_cast<size_t>(DrawList->VtxBuffer.Size) * sizeof(ImDrawVert);
		const size_t ListIndexBytes = static_cast<size_t>(DrawList->IdxBuffer.Size) * sizeof(ImDrawIdx);

		memcpy(VertexData + VertexWriteOffset, DrawList->VtxBuffer.Data, ListVertexBytes);
		memcpy(IndexData + IndexWriteOffset, DrawList->IdxBuffer.Data, ListIndexBytes);

		VertexWriteOffset += ListVertexBytes;
		IndexWriteOffset += ListIndexBytes;
	}

	m_VertexBuffer->Unmap();
	m_IndexBuffer->Unmap();

	const BackBufferTarget& Target = GetOrCreateBackBufferTarget(BackBuffer);

	const uint32_t Width = BackBuffer->GetWidth();
	const uint32_t Height = BackBuffer->GetHeight();

	auto& Command = VulkanCommand->GetNative();

	vk::RenderPassBeginInfo BeginInfo;
	BeginInfo.setRenderPass(m_RenderPass)
		.setFramebuffer(Target.FrameBuffer)
		.setRenderArea(vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(Width, Height)));

	Command.beginRenderPass(&BeginInfo, vk::SubpassContents::eInline);

	Command.bindPipeline(vk::PipelineBindPoint::eGraphics, m_Pipeline);
	Command.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_PipelineLayout, 0u, 1u, &m_DescriptorSet, 0u, nullptr);

	const vk::Buffer NativeVertexBuffer = static_cast<const VulkanBuffer*>(m_VertexBuffer.get())->GetNative();
	const vk::DeviceSize VertexBufferOffset = 0ull;
	Command.bindVertexBuffers(0u, 1u, &NativeVertexBuffer, &VertexBufferOffset);
	Command.bindIndexBuffer(
		static_cast<const VulkanBuffer*>(m_IndexBuffer.get())->GetNative(), 0ull,
		sizeof(ImDrawIdx) == sizeof(uint16_t) ? vk::IndexType::eUint16 : vk::IndexType::eUint32);

	/// The framebuffer carries device pixels, so this is the physical rectangle: the vertices below
	/// reach NDC through the projection and are stretched out to this by the viewport transform.
	const vk::Viewport Viewport(0.0f, 0.0f, static_cast<float>(Width), static_cast<float>(Height), 0.0f, 1.0f);
	Command.setViewport(0u, 1u, &Viewport);

	/// ImGui hands those vertices over in logical units, so the projection has to work in that same
	/// space, whatever the back buffer is sized in. (ImGui.vert assumes a DisplayPos of (0, 0),
	/// which is the case for us since multi viewports are turned off.)
	const float InverseDisplaySize[2u] =
	{
		1.0f / DrawData->DisplaySize.x,
		1.0f / DrawData->DisplaySize.y
	};
	Command.pushConstants(m_PipelineLayout, vk::ShaderStageFlagBits::eVertex, 0u, sizeof(InverseDisplaySize), InverseDisplaySize);

	/// ClipRect comes to us in that same logical space while the scissor below wants device pixels.
	/// A scale of zero can only mean nobody set it, and is treated as 1:1 like ImGui itself does.
	const ImVec2 ClipScale
	{
		DrawData->FramebufferScale.x > 0.0f ? DrawData->FramebufferScale.x : 1.0f,
		DrawData->FramebufferScale.y > 0.0f ? DrawData->FramebufferScale.y : 1.0f
	};

	uint32_t BaseVertex = 0u;
	uint32_t BaseIndex = 0u;
	for (int CmdListIndex = 0; CmdListIndex < DrawData->CmdListsCount; ++CmdListIndex)
	{
		const ImDrawList* DrawList = DrawData->CmdLists[CmdListIndex];

		for (int CmdIndex = 0; CmdIndex < DrawList->CmdBuffer.Size; ++CmdIndex)
		{
			const ImDrawCmd& DrawCmd = DrawList->CmdBuffer[CmdIndex];

			if (DrawCmd.UserCallback)
			{
				DrawCmd.UserCallback(DrawList, &DrawCmd);
				continue;
			}

			/// ClipRect is in the same space as the vertices, hence stretched to device pixels here.
			/// Clamping afterwards keeps a partially offscreen window from handing the driver a
			/// negative extent.
			const float MinX = std::max(DrawCmd.ClipRect.x * ClipScale.x, 0.0f);
			const float MinY = std::max(DrawCmd.ClipRect.y * ClipScale.y, 0.0f);
			const float MaxX = std::min(DrawCmd.ClipRect.z * ClipScale.x, static_cast<float>(Width));
			const float MaxY = std::min(DrawCmd.ClipRect.w * ClipScale.y, static_cast<float>(Height));

			if (MaxX <= MinX || MaxY <= MinY)
			{
				continue;
			}

			const vk::Rect2D Scissor(
				vk::Offset2D(static_cast<int32_t>(MinX), static_cast<int32_t>(MinY)),
				vk::Extent2D(static_cast<uint32_t>(MaxX - MinX), static_cast<uint32_t>(MaxY - MinY)));

			Command.setScissor(0u, 1u, &Scissor);
			Command.drawIndexed(
				DrawCmd.ElemCount, 1u,
				DrawCmd.IdxOffset + BaseIndex,
				static_cast<int32_t>(DrawCmd.VtxOffset + BaseVertex), 0u);
		}

		BaseVertex += static_cast<uint32_t>(DrawList->VtxBuffer.Size);
		BaseIndex += static_cast<uint32_t>(DrawList->IdxBuffer.Size);
	}

	Command.endRenderPass();
}
