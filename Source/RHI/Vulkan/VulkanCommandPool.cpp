#include "RHI/Vulkan/VulkanCommandPool.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanQueue.h"
#include "Engine/Services/SpdLogService.h"

VulkanCommandPool::VulkanCommandPool(const VulkanDevice& Device, uint32_t QueueFamilyIndex)
	: VkHwResource(Device)
{
	/*************************
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT:
			specifies that command buffers allocated from the pool will be short - lived,
			meaning that they will be reset or freed in a relatively short timeframe.
			This flag may be used by the implementation to control memory allocation behavior within the pool.

		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT:
			allows any command buffer allocated from a pool to be individually reset to the initial state;
			either by calling vkResetCommandBuffer, or via the implicit reset when calling vkBeginCommandBuffer.
			If this flag is not set on a pool, then vkResetCommandBuffer must not be called for any command buffer allocated from that pool.

		VK_COMMAND_POOL_CREATE_PROTECTED_BIT:
			specifies that command buffers allocated from the pool are protected command buffers.
			If the protected memory feature is not enabled, the VK_COMMAND_POOL_CREATE_PROTECTED_BIT bit of flags must not be set.
	**************************/

	auto vkCreateInfo = vk::CommandPoolCreateInfo()
		.setQueueFamilyIndex(QueueFamilyIndex)
		.setFlags(vk::CommandPoolCreateFlags());
	VERIFY_VK(GetNativeDevice().createCommandPool(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

std::shared_ptr<VulkanCommandBuffer> VulkanCommandPool::AllocateCommandBuffer(ERHICommandBufferLevel Level)
{
	auto vkAllocateInfo = vk::CommandBufferAllocateInfo()
		.setCommandBufferCount(1u)
		.setCommandPool(GetNative())
		.setLevel(GetCommandBufferLevel(Level));

	auto CommandBuffer = std::make_shared<VulkanCommandBuffer>(GetDevice(), *this, Level);
	VERIFY_VK(GetNativeDevice().allocateCommandBuffers(&vkAllocateInfo, &CommandBuffer->GetNative()));

	return CommandBuffer;
}

void VulkanCommandPool::Reset(bool ReleaseResources)
{
	/// Resetting a command pool recycles all of the resources from all of the command buffers allocated from the command pool back to the command pool. 
	/// All command buffers that have been allocated from the command pool are put in the initial state.
	/// Any primary command buffer allocated from another VkCommandPool that is in the recording or executable state and has a secondary command buffer allocated from commandPool recorded into it, becomes invalid.
	/// VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT specifies that resetting a command pool recycles all of the resources from the command pool back to the system.
	/// All VkCommandBuffer objects allocated from commandPool must not be in the pending state
	
	GetNativeDevice().resetCommandPool(GetNative(), ReleaseResources ? vk::CommandPoolResetFlagBits::eReleaseResources : vk::CommandPoolResetFlags{});
}

void VulkanCommandPool::Free(std::shared_ptr<VulkanCommandBuffer>& CommandBuffer)
{
	assert(CommandBuffer->GetState() != VulkanCommandBuffer::EState::Executable);
	
	std::vector<vk::CommandBuffer> CommandBuffers{ CommandBuffer->GetNative() };
	GetNativeDevice().freeCommandBuffers(GetNative(), CommandBuffers);
	CommandBuffer.reset();
}

VulkanCommandPool::~VulkanCommandPool()
{
	/// When a pool is destroyed, all command buffers allocated from the pool are freed.

	/// Any primary command buffer allocated from another VkCommandPool that is in the recording or 
	/// executable state and has a secondary command buffer allocated from commandPool recorded into it, becomes invalid.

	/// All VkCommandBuffer objects allocated from commandPool must not be in the pending state.

	Reset(true);
	GetNativeDevice().destroyCommandPool(GetNative());
}

VulkanCommandBufferManager::VulkanCommandBufferManager(const VulkanDevice& Device, const VulkanQueue& Queue)
	: m_Pool(Device, Queue.GetFamilyIndex())
{
	m_CommandBuffer = m_Pool.AllocateCommandBuffer(ERHICommandBufferLevel::Primary);
}
