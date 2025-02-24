#pragma once

#include "RHI/Vulkan/VulkanAsync.h"
#include "Engine/RHI/RHICommandBuffer.h"

/// Command buffers are objects used to record commands which can be subsequently submitted to a device queue for execution. 
/// There are two levels of command buffers - primary command buffers, which can execute secondary command buffers, and which are submitted to queues, 
/// and secondary command buffers, which can be executed by primary command buffers, and which are not directly submitted to queues.

/// Each command buffer manages state independently of other command buffers. There is no inheritance of state across primary and secondary command buffers, or between secondary command buffers. 
/// When a command buffer begins recording, all state in that command buffer is undefined. When secondary command buffer(s) are recorded to execute on a primary command buffer, 
/// the secondary command buffer inherits no state from the primary command buffer, and all state of the primary command buffer is undefined after an execute secondary command buffer command is recorded. 
/// There is one exception to this rule - if the primary command buffer is inside a render pass instance, then the render pass and subpass state is not disturbed by executing secondary command buffers. 
/// For state dependent commands (such as draws and dispatches), any state consumed by those commands must not be undefined.

/// Unless otherwise specified, and without explicit synchronization, the various commands submitted to a queue via command buffers may execute in arbitrary order relative to each other, and/or concurrently. 
/// Also, the memory side-effects of those commands may not be directly visible to other commands without explicit memory dependencies. This is true within a command buffer, and across command buffers submitted to a given queue. 

class VulkanCommandBuffer final : public VkDeviceResource<vk::CommandBuffer>, public RHICommandBuffer
{
public:
	VulkanCommandBuffer(const class VulkanDevice& Device, class VulkanCommandPool& Pool, ERHICommandBufferLevel Level);

	void Begin() override final;
	void End() override final;
	void Reset() override final;

	void BeginDebugMarker(const char* Name, const Math::Color& Markercolor) override final;
	void EndDebugMarker() override final;

	void SetVertexBuffer(const RHIBuffer* Buffer, uint32_t StartSlot, size_t Offset) override final;
	void SetVertexStream(uint32_t StartSlot, const RHIVertexStream& VertexStream) override final;
	void SetIndexBuffer(const RHIBuffer* Buffer, size_t Offset, ERHIIndexFormat IndexFormat) override final;
	void SetPrimitiveTopology(ERHIPrimitiveTopology Topology);

	void Draw(uint32_t VertexCount, uint32_t FirstVertex) override final;
	void DrawInstanced(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance) override final;
	void DrawIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) override final;

	void DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, int32_t VertexOffset) override final;
	void DrawIndexedInstanced(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, int32_t VertexOffset, uint32_t FirstInstance) override final;
	void DrawIndexedIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) override final;

	void Dispatch(uint32_t GroupX, uint32_t GroupY, uint32_t GroupZ) override final;
	void DispatchIndirect(const RHIBuffer* IndirectBuffer, size_t Offset) override final;

	void PushConstants(ERHIShaderStage Stage, const RHIBuffer* Buffer, const void* Data, size_t Size, size_t Offset) override final;

	void SetGraphicsPipeline(const RHIGraphicsPipeline* GraphicsPipeline) override final;

	void ClearColorTexture(const RHITexture* Texture, const Math::Color& ClearColor) override final;

	void ClearDepthStencilTexture(const RHITexture* Texture, bool ClearDepth, bool ClearStencil, float Depth, uint8_t Stencil) override final;

	void WriteBuffer(const RHIBuffer* Buffer, const void* Data, size_t Size, size_t Offset) override final;
	void WriteTexture(const RHITexture* Texture, const void* Data, size_t Size, size_t Offset) override final;

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetViewports(const RHIViewport* Viewports, uint32_t NumViewports) override final;

	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;
	void SetScissorRects(const RHIScissorRect* ScissorRects, uint32_t NumScissorRects) override final;

	VulkanFence* GetFence() { return &m_Fence; }
	const std::vector<VulkanSemaphore*> GetWaitSemaphores() const { return m_WaitSemaphores; }
	const std::vector<vk::PipelineStageFlags>& GetWaitDstStageFlags() const { return m_WaitDstStageFlags; }
protected:
	friend class VulkanCommandListContext;
private:
	class VulkanCommandPool& m_Pool;
	VulkanFence m_Fence;
	std::vector<VulkanSemaphore*> m_WaitSemaphores;
	std::vector<vk::PipelineStageFlags> m_WaitDstStageFlags;
};