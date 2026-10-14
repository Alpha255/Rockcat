#pragma once

#include "RHI/Vulkan/VulkanLoader.h"
#include "Rendering/RHI/RHIBuffer.h"
#include "Rendering/RHI/RHITexture.h"

#include <unordered_map>

/***************************************************************************************************
	Self contained Dear ImGui renderer for the Vulkan backend.

	It deliberately builds its own shader modules, descriptor set layout, pipeline layout, render
	pass and pipeline instead of going through the generic RHI path: that path derives its
	descriptor set layout from Shader::GetVariables(), which is populated by the reflection step of
	a shader compilation service that is currently disabled (see ShaderLibrary.cpp). Wiring the UI
	through it would mean reviving the whole compilation + reflection chain first.

	Usage:

		m_ImGuiRenderer.Init(GetRenderDevice(), SwapchainColorFormat);
		...
		ImGui::NewFrame();
		ImGui::Render();
		m_ImGuiRenderer.RenderDrawData(CommandBuffer, Swapchain->GetBackBuffer());
		Swapchain->TransitionToPresent(CommandBuffer);
***************************************************************************************************/
class VulkanImGuiRenderer
{
public:
	~VulkanImGuiRenderer();

	/// ColorFormat is the VkFormat of the swap chain images the UI is drawn into.
	bool Init(const class RHIDevice& Device, vk::Format ColorFormat);

	void Shutdown();

	/// Recording assumes BackBuffer holds whatever ClearColorTexture() wrote, hence the layout note
	/// above. IO.DisplaySize is expected in logical units with IO.DisplayFramebufferScale carrying
	/// the device pixel ratio, so the vertices stay authored the way ImGui lays them out while the
	/// viewport and the scissor rects are pushed out to full device resolution.
	void RenderDrawData(class RHICommandBuffer* CommandBuffer, const class RHITexture* BackBuffer);

	/// Re-reads the atlas ImGui rebuilt - which is what a density change does - and puts it back on
	/// the GPU. Has to run between frames, never inside one, since it drops what this frame draws
	/// the text with.
	bool RebuildFontAtlas(const class RHIDevice& Device);

	/// Drops every cached image view and frame buffer. Those reference images owned by the swap
	/// chain, and Vulkan requires a view to be gone before the image it points at, so this has to
	/// run before anything rebuilds the chain.
	void InvalidateBackBufferCache();

	inline bool IsInitialized() const { return m_Pipeline != nullptr; }

private:
	bool CreateShaderModules();
	void CreateLayouts();
	void CreateRenderPass();
	bool CreateFontResources(const class RHIDevice& Device);
	void DestroyFontResources();
	bool CreatePipeline();

	/// One image view + frame buffer per back buffer, together with the extent they were built for
	/// so that a rebuilt swap chain is picked up instead of drawn into.
	struct BackBufferTarget
	{
		vk::ImageView View;
		vk::Framebuffer FrameBuffer;
		uint32_t Width = 0u;
		uint32_t Height = 0u;
	};

	/// Keyed on the RHITexture rather than the VkImage handle: VulkanSwapchain::Create() allocates
	/// fresh VulkanTexture objects on resize, so a recycled VkImage handle can never be mistaken
	/// for a live one.
	const BackBufferTarget& GetOrCreateBackBufferTarget(const class RHITexture* Texture);
	void DestroyBackBufferTarget(BackBufferTarget& Target);

	void ResizeBuffer(RHIBufferPtr& Buffer, size_t& CurrentBytes, size_t RequiredBytes, ERHIBufferUsageFlags Usages, const char* DebugName);

	const vk::Device& GetNativeDevice() const;

	const class VulkanDevice* m_Device = nullptr;

	vk::Format m_ColorFormat = vk::Format::eUndefined;

	vk::ShaderModule m_VertexShaderModule;
	vk::ShaderModule m_FragmentShaderModule;

	vk::DescriptorSetLayout m_DescriptorSetLayout;
	vk::PipelineLayout m_PipelineLayout;
	vk::RenderPass m_RenderPass;
	vk::Pipeline m_Pipeline;

	RHISamplerPtr m_FontSampler;
	RHITexturePtr m_FontTexture;
	vk::ImageView m_FontImageView;

	vk::DescriptorPool m_DescriptorPool;
	vk::DescriptorSet m_DescriptorSet;

	/// Host visible + host coherent: the draw data is memcpy'd in every frame and read once by the
	/// GPU, so there is nothing to gain from an extra staging copy.
	RHIBufferPtr m_VertexBuffer;
	RHIBufferPtr m_IndexBuffer;
	size_t m_VertexBufferBytes = 0ull;
	size_t m_IndexBufferBytes = 0ull;

	std::unordered_map<const RHITexture*, BackBufferTarget> m_BackBufferTargets;
};
