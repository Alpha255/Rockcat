#if 0
#include "Colorful/Vulkan/VulkanQueue.h"
#include "Colorful/Vulkan/VulkanDevice.h"
#include "Colorful/Vulkan/VulkanSwapchain.h"

NAMESPACE_START(RHI)

VulkanQueue::VulkanQueue(VulkanDevice* Device, EQueueType QueueType, uint32_t QueueFamilyIndex)
	: VkHWObject(Device)
	, m_Type(QueueType)
	, m_QueueFamilyIndex(QueueFamilyIndex)
	, m_CompleteSemaphore(std::move(std::make_unique<VulkanSemaphore>(Device)))
{
	/// vkGetDeviceQueue must only be used to get queues that were created with the flags parameter of VkDeviceQueueCreateInfo set to zero. 
	/// To get queues that were created with a non-zero flags parameter use vkGetDeviceQueue2.

	/// All queues associated with a logical device are destroyed when vkDestroyDevice is called on that device

	/// queueIndex must be less than the value of VkDeviceQueueCreateInfo::queueCount for the queue family indicated by queueFamilyIndex when device was created
	assert(m_QueueFamilyIndex != ~0u);
	vkGetDeviceQueue(m_Device->Get(), m_QueueFamilyIndex, 0u, Reference());
}

std::shared_ptr<VulkanCommandBuffer> VulkanQueue::GetOrAllocateCommandBuffer(ECommandBufferLevel Level, bool8_t AutoBegin, bool8_t UseForTransfer)
{
	auto ThreadID = std::this_thread::get_id();
	auto CommandPool = m_CmdPools.second.end();

	{
		std::lock_guard<std::mutex> Scopedocker(m_CmdPools.first);

		CommandPool = std::find_if(m_CmdPools.second.begin(), m_CmdPools.second.end(), [ThreadID](const VkCommandBufferPool& Pool) {
			return ThreadID == Pool.first;
			});

		if (CommandPool == m_CmdPools.second.end())
		{
			m_CmdPools.second.emplace_back(std::make_pair(ThreadID, std::make_unique<VulkanCommandBufferPool>(
				m_Device,
				m_QueueFamilyIndex,
				m_Type,
				VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
				)));

			CommandPool = --m_CmdPools.second.end();
		}
	}

	return CommandPool->second->GetOrAllocate(Level, AutoBegin, UseForTransfer);
}

void VulkanQueue::QueueSubmit(const std::vector<ICommandBufferSharedPtr>& CommandBuffers)
{
	std::lock_guard<std::mutex> ScopedLocker(m_CommandsTracker.Mutex);

	for (auto Command : CommandBuffers)
	{
		m_CommandsTracker.Queued.emplace_back(Command);
	}
}

void VulkanQueue::SubmitQueuedCommandBuffers()
{
	if (!m_CommandsTracker.Queued.empty())
	{
		Submit(m_CommandsTracker.Queued);

		m_CommandsTracker.Queued.clear();
	}
}

void VulkanQueue::Submit(const std::vector<ICommandBufferSharedPtr>& CommandBuffers)
{
	/// #TODO: Async submit

	assert(CommandBuffers.size() > 0u);

	std::vector<VkCommandBuffer> NativeCommands(CommandBuffers.size());
	std::vector<VulkanCommandBuffer*> VkCommands(CommandBuffers.size());

	for (uint32_t CommandIndex = 0u; CommandIndex < CommandBuffers.size(); ++CommandIndex)
	{
		auto VkCommand = std::static_pointer_cast<VulkanCommandBuffer>(CommandBuffers[CommandIndex]);
		assert(VkCommand && VkCommand->Pool()->QueueType() == m_Type);

		if (VkCommand->State() == VulkanCommandBuffer::EState::Recording)
		{
			VkCommand->End();
		}

		VkCommands[CommandIndex] = VkCommand.get();
		NativeCommands[CommandIndex] = VkCommand->Get();

#if 0
		auto& WaitSemaphores = VkCommands[i]->WaitSemaphores();
		m_WaitSemaphores.insert(m_WaitSemaphores.end(), WaitSemaphores.begin(), WaitSemaphores.end());
#endif

		m_CommandsTracker.InFlight.push_back(VkCommand);
	}

	VkSubmitInfo SubmitInfo
	{
		VK_STRUCTURE_TYPE_SUBMIT_INFO,
		nullptr,
		static_cast<uint32_t>(m_WaitSemaphores.size()),
		m_WaitSemaphores.data(),
		m_WaitDstStageMasks.data(),
		static_cast<uint32_t>(NativeCommands.size()),
		NativeCommands.data(),
		static_cast<uint32_t>(m_SignalSemaphores.size()),
		m_SignalSemaphores.data()
	};

	auto FenceToSignal = m_Device->GetOrCreateFence();

#if VK_KHR_timeline_semaphore
	if (m_Device->EnabledExtensions().TimelineSemaphore)
	{
		VkTimelineSemaphoreSubmitInfo TimelineSemaphoreSubmitInfo
		{
			VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO_KHR,
			nullptr,
			static_cast<uint32_t>(m_WaitSemaphores.size()),
			nullptr,
			static_cast<uint32_t>(m_SignalSemaphores.size()),
			nullptr
		};
		SubmitInfo.pNext = &TimelineSemaphoreSubmitInfo;
	}
#endif

	VERIFY_VK(vkQueueSubmit(m_Handle, 1u, &SubmitInfo, FenceToSignal->Get()));

	m_WaitSemaphores.clear();
	m_WaitDstStageMasks.clear();
	m_SignalSemaphores.clear();

	for (auto Command : VkCommands)
	{
		Command->SetState(VulkanCommandBuffer::EState::Pending);
		Command->BindSubmittedFence(FenceToSignal);
	}

	Sync();
}

void VulkanQueue::Sync()
{
	if (m_Device->Options().SyncType == ESyncType::SyncByFence)
	{
		for (auto CommandIt = m_CommandsTracker.InFlight.begin(); CommandIt != m_CommandsTracker.InFlight.end();)
		{
			auto& Command = *CommandIt;
			auto Fence = Command->SubmittedFence();

			if (Command->IsUseForTransfer() && !Fence->IsSignaled())
			{
				Fence->Wait(std::numeric_limits<uint64_t>().max());
			}

			if (Fence->IsSignaled())
			{
				Command->Pool()->Recycle(Command);

				if (Fence.use_count() == 2u)
				{
					m_Device->FreeFence(Fence);
				}

				CommandIt = m_CommandsTracker.InFlight.erase(CommandIt);
				continue;
			}

			++CommandIt;
		}
	}
}

void VulkanQueue::WaitIdle()
{
	/**************************************************************************************************************************
		vkQueueWaitIdle is equivalent to having submitted a valid fence to every previously executed queue submission command that accepts a fence, 
		then waiting for all of those fences to signal using vkWaitForFences with an infinite timeout and waitAll set to VK_TRUE.
	***************************************************************************************************************************/

	VERIFY_VK(vkQueueWaitIdle(m_Handle));
}

VulkanQueue::~VulkanQueue()
{
	for (auto& Pool : m_CmdPools.second)
	{
		Pool.second->Reset();
	}
	m_CmdPools.second.clear();

	WaitIdle();
}

NAMESPACE_END(RHI)
#endif
