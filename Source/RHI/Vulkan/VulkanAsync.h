#pragma once

#include "RHI/Vulkan/VulkanTypes.h"

/*****************
	Fences
		Fences can be used to communicate to the host that execution of some task on the device has completed.
		Fences are a synchronization primitive that can be used to insert a dependency from a queue to the host.

	Semaphores
		Semaphores can be used to control resource access across multiple queues.
		Semaphores are a synchronization primitive that can be used to insert a dependency between batches submitted to queues.

	Events
		Events provide a fine-grained synchronization primitive which can be signaled either within a command buffer or by the host, and can be waited upon within a command buffer or queried on the host.
		Events are a synchronization primitive that can be used to insert a fine-grained dependency between commands submitted to the same queue, 
		or between the host and a queue. Events must not be used to insert a dependency between commands submitted to different queues.

	Pipeline Barriers
		Pipeline barriers also provide synchronization control within a command buffer, but at a single point, rather than with separate signal and wait operations.
		vkCmdPipelineBarrier is a synchronization command that inserts a dependency between commands submitted to the same queue, or between commands in the same subpass.

	Memory Barriers
		Memory barriers are used to explicitly control access to buffer and image subresource ranges. 
		Memory barriers are used to transfer ownership between queue families, change image layouts, and define availability and visibility operations. 
		They explicitly define the access types and buffer and image subresource ranges that are included in the access scopes of a memory dependency that is created by a 
		synchronization command that includes them.
		VkMemoryBarrier/VkBufferMemoryBarrier/VkImageMemoryBarrier 

	Render Passes
		Render passes provide a useful synchronization framework for most rendering tasks, built upon the concepts in this chapter. 
		Many cases that would otherwise need an application to use other synchronization primitives can be expressed more efficiently as part of a render pass.
********************/

class VulkanFence final : public VkHwResource<vk::Fence>
{
public:
	VulkanFence(const class VulkanDevice& Device, bool8_t Signaled = false);

	~VulkanFence() = default;

	bool8_t IsSignaled() const;

	void Reset() const;

	void Wait(uint64_t Nanoseconds) const;
protected:
private:
};

class VulkanSemaphore final : public VkHwResource<vk::Semaphore>
{
public:
	VulkanSemaphore(const class VulkanDevice& Device);

	~VulkanSemaphore() = default;

	uint64_t GetCounterValue() const;

	void Wait(uint64_t Value, uint64_t Nanoseconds) const;

	void Signal(uint64_t Value) const;
};

class VulkanEvent final : public VkHwResource<vk::Event>
{
public:
	VulkanEvent(const class VulkanDevice& Device);

	~VulkanEvent() = default;

	bool8_t IsSignaled() const;

	void Signal(bool8_t Signaled) const;
};

#if 0
struct VulkanPipelineBarrier
{
	VkPipelineStageFlags SrcStageFlags = VK_PIPELINE_STAGE_NONE_KHR;
	VkPipelineStageFlags DstStageFlags = VK_PIPELINE_STAGE_NONE_KHR;

	VulkanPipelineBarrier(class VulkanCommandBuffer* Command)
		: GlobalMemoryBarrier(
			VkMemoryBarrier
			{
				VK_STRUCTURE_TYPE_MEMORY_BARRIER,
				nullptr,
				VK_ACCESS_NONE_KHR,
				VK_ACCESS_NONE_KHR
			})
		, m_Command(Command)
	{
		assert(m_Command != VK_NULL_HANDLE);
	}

	VkMemoryBarrier GlobalMemoryBarrier;
	std::vector<VkImageMemoryBarrier> ImageBarriers;
	std::vector<VkBufferMemoryBarrier> BufferBarriers;

	VulkanPipelineBarrier& TransitionResourceState(class VulkanImage* Image, EResourceState Src, EResourceState Dst, const VkImageSubresourceRange& SubresourceRange);
	VulkanPipelineBarrier& TransitionResourceState(class VulkanBuffer* Buffer, EResourceState Src, EResourceState Dst);

	void Commit();

	static VkAccessFlags AccessFlags(VkImageLayout Layout);

	static VkPipelineStageFlags PipelineStageMask(VkImageLayout Layout);

	static VkPipelineStageFlags PipelineStageMask(VkAccessFlags AccessMask);

	struct VkResourceState : public std::tuple<VkPipelineStageFlags, VkAccessFlags, VkImageLayout>
	{
		using Super = std::tuple<VkPipelineStageFlags, VkAccessFlags, VkImageLayout>;
		using Super::Super;

		const VkPipelineStageFlags StageFlags() const
		{
			return std::get<0>(*this);
		}

		const VkAccessFlags AccessFlags() const
		{
			return std::get<1>(*this);
		}

		const VkImageLayout ImageLayout() const
		{
			return std::get<2>(*this);
		}
	};

	static VkResourceState TranslateResourceState(EResourceState State);
protected:
	class VulkanCommandBuffer* m_Command = nullptr;
};
#endif
