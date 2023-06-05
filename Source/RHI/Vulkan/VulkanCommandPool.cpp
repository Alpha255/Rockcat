#if 0
#include "Colorful/Vulkan/VulkanCommandPool.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(RHI)

VulkanCommandBufferPool::VulkanCommandBufferPool(VulkanDevice* Device, uint32_t QueueFamilyIndex, EQueueType QueueType, VkCommandPoolCreateFlags CreateFlags)
	: VkHWObject(Device)
	, m_QueueType(QueueType)
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
	VkCommandPoolCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		nullptr,
		CreateFlags,
		QueueFamilyIndex  /// All command buffers allocated from this command pool must be submitted on queues from the same queue family.
	};
	VERIFY_VK(vkCreateCommandPool(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));
}

std::shared_ptr<VulkanCommandBuffer> VulkanCommandBufferPool::GetOrAllocate(ECommandBufferLevel Level, bool8_t AutoBegin, bool8_t UseForTransfer)
{
	std::shared_ptr<VulkanCommandBuffer> Command = nullptr;
	auto& CommandList = Level == ECommandBufferLevel::Primary ? m_PrimaryCommandBuffers : m_SecondaryCommandBuffers;
	if (!CommandList.empty())
	{
		Command = CommandList.front();
		CommandList.pop_front();
	}
	else
	{
		VkCommandBufferAllocateInfo AllocateInfo
		{
			VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			nullptr,
			m_Handle,
			Level == ECommandBufferLevel::Primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
			1u
		};

		Command = std::make_shared<VulkanCommandBuffer>(m_Device, this, Level, UseForTransfer);
		VERIFY_VK(vkAllocateCommandBuffers(m_Device->Get(), &AllocateInfo, Command->Reference()));
	}

	assert(Command && (Command->State() == VulkanCommandBuffer::EState::Initial || Command->State() == VulkanCommandBuffer::EState::Invalid));
	if (AutoBegin)
	{
		Command->Begin();
	}

	return Command;
}

void VulkanCommandBufferPool::Reset()
{
	/// Resetting a command pool recycles all of the resources from all of the command buffers allocated from the command pool back to the command pool. 
	/// All command buffers that have been allocated from the command pool are put in the initial state.
	/// Any primary command buffer allocated from another VkCommandPool that is in the recording or executable state and has a secondary command buffer allocated from commandPool recorded into it, becomes invalid.
	/// VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT specifies that resetting a command pool recycles all of the resources from the command pool back to the system.
	/// All VkCommandBuffer objects allocated from commandPool must not be in the pending state

	VERIFY_VK(vkResetCommandPool(m_Device->Get(), Get(), VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT));
}

void VulkanCommandBufferPool::Recycle(std::shared_ptr<VulkanCommandBuffer>& Command)
{
	if (Command)
	{
		Command->SetState(VulkanCommandBuffer::EState::Executable);
		Command->Reset();

		if (Command->Level() == ECommandBufferLevel::Primary)
		{
			m_PrimaryCommandBuffers.emplace_back(Command);
		}
		else
		{
			m_SecondaryCommandBuffers.emplace_back(Command);
		}
	}
}

void VulkanCommandBufferPool::Free(std::shared_ptr<VulkanCommandBuffer>& Command)
{
	assert(Command->State() != VulkanCommandBuffer::EState::Pending);

	const VkCommandBuffer Commands[]{ Command->Get() };
	vkFreeCommandBuffers(m_Device->Get(), m_Handle, 1u, Commands);

	Command->Reset();
}

VulkanCommandBufferPool::~VulkanCommandBufferPool()
{
	/// When a pool is destroyed, all command buffers allocated from the pool are freed.

	/// Any primary command buffer allocated from another VkCommandPool that is in the recording or 
	/// executable state and has a secondary command buffer allocated from commandPool recorded into it, becomes invalid.

	/// All VkCommandBuffer objects allocated from commandPool must not be in the pending state.

	Reset();

#if _DEBUG
	for (auto& Command : m_PrimaryCommandBuffers)
	{
		assert(Command->State() != VulkanCommandBuffer::EState::Pending);
	}
	for (auto& Command : m_SecondaryCommandBuffers)
	{
		assert(Command->State() != VulkanCommandBuffer::EState::Pending);
	}
#endif
	m_PrimaryCommandBuffers.clear();
	m_SecondaryCommandBuffers.clear();

	vkDestroyCommandPool(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

NAMESPACE_END(RHI)
#endif