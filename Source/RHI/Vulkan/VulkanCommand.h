#pragma once

#include "Colorful/Vulkan/VulkanAsync.h"

NAMESPACE_START(RHI)
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

class VulkanCommandBuffer final : public VkHWObject<ICommandBuffer, VkCommandBuffer_T>
{
public:
	enum class EState
	{
		Initial,
		Recording,
		Executable,
		Pending,
		Invalid
	};

	VulkanCommandBuffer(class VulkanDevice* Device, class VulkanCommandBufferPool* Pool, ECommandBufferLevel Level, bool8_t UseForTransfer = false);

	inline EState State() const
	{
		return m_State;
	}

	void Begin() override final;
	void End() override final;
	void Reset() override final;

	void BeginRenderPass(IFrameBuffer* FrameBuffer);
	void EndRenderPass();

	void BeginDebugMarker(const char8_t* Name, const Color& Markercolor) override final;
	void EndDebugMarker() override final;

	void Draw(uint32_t VertexCount, uint32_t FirstVertex) override final;
	void DrawInstanced(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance) override final;
	void DrawIndirect(IBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) override final;

	void DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, int32_t VertexOffset) override final;
	void DrawIndexedInstanced(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, int32_t VertexOffset, uint32_t FirstInstance) override final;
	void DrawIndexedIndirect(IBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) override final;

	void Dispatch(uint32_t GroupX, uint32_t GroupY, uint32_t GroupZ) override final;
	void DispatchIndirect(IBuffer* IndirectBuffer, size_t Offset) override final;

	void PushConstants(EShaderStage Stage, IBuffer* ConstantsBuffer, const void* Data, size_t Size, size_t Offset = 0u) override final;

	void SetGraphicsPipeline(IPipeline* DstPipeline) override final;

	void ClearColorImage(IImage* DstImage, const Color& ClearColor) override final;

	void ClearDepthStencilImage(IImage* DstImage, bool8_t ClearDepth, bool8_t ClearStencil, float32_t Depth, uint8_t Stencil) override final;

	void CopyBufferToImage(IImage* DstImage, const void* SrcBuffer, uint32_t BufferSize, const ImageSubresourceRange& SubresourceRange) override final;

	void CopyBuffer(IBuffer* DstBuffer, const void* Data, size_t DataSize, size_t SrcOffset, size_t DstOffset) override final;

	void CopyImage(IImage* SrcImage, const ImageSlice& SrcSlice, IImage* DstImage, const ImageSlice& DstSlice) override final;

	void SetViewport(const Viewport& DstViewport) override final;

	void SetScissorRect(const ScissorRect& DstScissorRect) override final;

	void SetPolygonMode(EPolygonMode Mode);

	void ExecuteSecondaryCommands(const std::vector<std::shared_ptr<VulkanCommandBuffer>>& Commands);

	void WaitCommand(ICommandBuffer* CommandToWait) override final;

	bool8_t IsUseForTransfer() const
	{
		return m_UseForTransfer;
	}

	bool8_t IsInsideRenderpass() const
	{
		return m_InsideRenderPass;
	}

	VulkanCommandBufferPool* Pool() const
	{
		return m_Pool;
	}

	VkSemaphore Semaphore() const
	{
		return m_Semaphore->Get();
	}

	std::vector<VkSemaphore>& WaitSemaphores()
	{
		return m_WaitSemaphores;
	}

	void BindSubmittedFence(std::shared_ptr<VulkanFence>& Fence)
	{
		m_SubmittedFence = Fence;
	}

	std::shared_ptr<VulkanFence>& SubmittedFence()
	{
		return m_SubmittedFence;
	}
protected:
	friend class VulkanCommandBufferPool;
	friend class VulkanQueue;

	struct VkResourceStateTracker
	{
		std::unordered_set<class VulkanImage*> Images;
		std::unordered_set<class VulkanBuffer*> Buffers;
		std::unordered_set<class VulkanImage*> PermanentImages;

		void EnsureCorrectStates(VulkanPipelineBarrier* Barrier);
	};

	inline void SetState(EState State)
	{
		m_State = State;
	}

	void EnsurePipelineResourceStates(class VulkanGraphicsPipelineState* State);
private:
	EState m_State = EState::Initial;
	bool8_t m_InsideRenderPass = false;
	bool8_t m_UseForTransfer = false;
	class VulkanCommandBufferPool* m_Pool = nullptr;
	class VulkanGraphicsPipeline* m_CurGfxPipeline = nullptr;
	std::unique_ptr<VulkanPipelineBarrier> m_Barrier;
	std::unique_ptr<VulkanSemaphore> m_Semaphore;
	std::shared_ptr<VulkanFence> m_SubmittedFence;
	std::vector<VkSemaphore> m_WaitSemaphores;
	VkResourceStateTracker m_Tracker;
};

NAMESPACE_END(RHI)