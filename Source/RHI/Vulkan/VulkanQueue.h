#pragma once

#include "Colorful/Vulkan/VulkanBuffer.h"
#include "Colorful/Vulkan/VulkanCommandPool.h"
#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDescriptor.h"

NAMESPACE_START(RHI)

class VulkanQueue : public VkHWObject<void, VkQueue_T>
{
public:
	VulkanQueue(class VulkanDevice* Device, EQueueType QueueType, uint32_t QueueFamilyIndex);

	~VulkanQueue();

	const uint32_t FamilyIndex() const
	{
		return m_QueueFamilyIndex;
	}

	std::shared_ptr<VulkanCommandBufferPool> CreateCommandBufferPool() const
	{
		return std::make_shared<VulkanCommandBufferPool>(
			m_Device,
			m_QueueFamilyIndex,
			m_Type,
			VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
			);
	}

	std::shared_ptr<VulkanCommandBuffer> GetOrAllocateCommandBuffer(ECommandBufferLevel Level, bool8_t AutoBegin, bool8_t UserForTransfer);

	void AddWaitSemaphore(VkSemaphore Semaphore, VkPipelineStageFlags WaitDstStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT)
	{
		if (Semaphore)
		{
			m_WaitSemaphores.push_back(Semaphore);
			m_WaitDstStageMasks.push_back(WaitDstStageMask);
		}
	}

	void AddSignalSemaphore(VkSemaphore Semaphore)
	{
		if (Semaphore)
		{
			m_SignalSemaphores.push_back(Semaphore);
		}
	}

	void Submit(const std::vector<ICommandBufferSharedPtr>& CommandBuffers);

	void QueueSubmit(const std::vector<ICommandBufferSharedPtr>& CommandBuffers);

	void SubmitQueuedCommandBuffers();

	void Sync();

	void WaitIdle();
protected:
	using VkCommandBufferPool = std::pair<std::thread::id, std::unique_ptr<VulkanCommandBufferPool>>;

	struct CommandBufferTracker
	{
		std::list<std::shared_ptr<VulkanCommandBuffer>> InFlight;
		std::vector<ICommandBufferSharedPtr> Queued;
		std::mutex Mutex;
	};
private:
	EQueueType m_Type = EQueueType::Graphics;
	uint32_t m_QueueFamilyIndex = ~0u;
	std::pair<std::mutex, std::vector<VkCommandBufferPool>> m_CmdPools;  /// #TODO Use std::shared_mutex to impove performance???
	std::unique_ptr<VulkanSemaphore> m_CompleteSemaphore;
	std::vector<VkSemaphore> m_WaitSemaphores;
	std::vector<VkPipelineStageFlags> m_WaitDstStageMasks;
	std::vector<VkSemaphore> m_SignalSemaphores;
	CommandBufferTracker m_CommandsTracker;
};

NAMESPACE_END(RHI)
