#include "Colorful/Vulkan/VulkanAsync.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(RHI)

VulkanFence::VulkanFence(VulkanDevice* Device, bool8_t Signaled)
	: VkHWObject(Device)
{
	VkFenceCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		nullptr,
		Signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0u
	};

	VERIFY_VK(vkCreateFence(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));
}

bool8_t VulkanFence::IsSignaled()
{
	/// If a queue submission command is pending execution, then the value returned by this command may immediately be out of date.
	/// If the device has been lost, vkGetFenceStatus may return any of the (VK_SUCCESS|VK_NOT_READY|VK_ERROR_DEVICE_LOST). 
	/// If the device has been lost and vkGetFenceStatus is called repeatedly, it will eventually return either VK_SUCCESS or VK_ERROR_DEVICE_LOST.

	auto Ret = vkGetFenceStatus(m_Device->Get(), Get());
	if (VK_SUCCESS == Ret)
	{
		return true;
	}
	else if (VK_NOT_READY == Ret)
	{
		return false;
	}
	else if (VK_ERROR_DEVICE_LOST == Ret)
	{
		assert(false);
		return false;
	}

	return false;
}

void VulkanFence::Reset()
{
	VERIFY_VK(vkResetFences(m_Device->Get(), 1u, Reference()));
}

void VulkanFence::Wait(uint64_t Nanoseconds)
{
	VERIFY_VK(vkWaitForFences(m_Device->Get(), 1u, Reference(), true, Nanoseconds));
}

VulkanFence::~VulkanFence()
{
	vkDestroyFence(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

VulkanSemaphore::VulkanSemaphore(VulkanDevice* Device)
	: VkHWObject(Device)
{
	/// Semaphores have two states - signaled and unsignaled. The state of a semaphore can be signaled after execution of a batch of commands is completed. 
	/// A batch can wait for a semaphore to become signaled before it begins execution, and the semaphore is also unsignaled before the batch begins execution.

	VkSemaphoreCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		nullptr,
		0u  /// flags is reserved for future use
	};

#if VK_KHR_timeline_semaphore
	if (m_Device->EnabledExtensions().TimelineSemaphore)
	{
		VkSemaphoreTypeCreateInfo TypeCreateInfo
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
			nullptr,
			VK_SEMAPHORE_TYPE_TIMELINE,
			0u
		};
		CreateInfo.pNext = &TypeCreateInfo;
	}
#endif

	VERIFY_VK(vkCreateSemaphore(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));

	/// When a batch is submitted to a queue via a queue submission, and it includes semaphores to be signaled, 
	/// it defines a memory dependency on the batch, and defines semaphore signal operations which set the semaphores to the signaled state.

	/// Before waiting on a semaphore, the application must ensure the semaphore is in a valid state for a wait operation. Specifically, 
	/// when a semaphore wait and unsignal operation is submitted to a queue:
	/// 1. The semaphore must be signaled, or have an associated semaphore signal operation that is pending execution.
	/// 2. There must be no other queue waiting on the same semaphore when the operation executes.
}

uint64_t VulkanSemaphore::CounterValue() const
{
#if VK_KHR_timeline_semaphore
	if (m_Device->EnabledExtensions().TimelineSemaphore)
	{
		uint64_t Count = 0u;
		vkGetSemaphoreCounterValue(m_Device->Get(), m_Handle, &Count);
		return Count;
	}
#endif

	return std::numeric_limits<uint64_t>().max();
}

void VulkanSemaphore::Wait(uint64_t Value, uint64_t Nanoseconds)
{
#if VK_KHR_timeline_semaphore
	if (m_Device->EnabledExtensions().TimelineSemaphore)
	{
		VkSemaphoreWaitInfo WaitInfo
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
			nullptr,
			0u,
			1u,
			&m_Handle,
			&Value
		};
		VERIFY_VK(vkWaitSemaphores(m_Device->Get(), &WaitInfo, Nanoseconds));
	}
#endif
}

void VulkanSemaphore::Signal(uint64_t Value)
{
#if VK_KHR_timeline_semaphore
	if (m_Device->EnabledExtensions().TimelineSemaphore)
	{
		VkSemaphoreSignalInfo SignalInfo
		{
			VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO,
			nullptr,
			m_Handle,
			Value
		};
		VERIFY_VK(vkSignalSemaphore(m_Device->Get(), &SignalInfo));
	}
#endif
}

VulkanSemaphore::~VulkanSemaphore()
{
	vkDestroySemaphore(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

VulkanEvent::VulkanEvent(VulkanDevice* Device)
	: VkHWObject(Device)
{
	/// An application can signal an event, or unsignal it, on either the host or the device. 
	/// A device can wait for an event to become signaled before executing further operations. 
	/// No command exists to wait for an event to become signaled on the host.

	VkEventCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_EVENT_CREATE_INFO,
		nullptr,
		0u  /// flags is reserved for future use
	};

	VERIFY_VK(vkCreateEvent(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));

	/// The state of an event can also be updated on the device by commands inserted in command buffers.
	/// To set the state of an event to signaled from a device, call: vkCmdSetEvent
}

bool8_t VulkanEvent::IsSignaled()
{
	auto Ret = vkGetEventStatus(m_Device->Get(), Get());
	if (VK_EVENT_SET == Ret)
	{
		return true;
	}
	else if (VK_EVENT_RESET == Ret)
	{
		return false;
	}

	return false;
}
void VulkanEvent::Signal(bool8_t Signaled)
{
	if (Signaled)
	{
		/// Set the state of an event to signaled from the host
		VERIFY_VK(vkSetEvent(m_Device->Get(), Get()));
	}
	else
	{
		/// Set the state of an event to unsignaled from the host
		VERIFY_VK(vkResetEvent(m_Device->Get(), Get()));
	}
}

VulkanEvent::~VulkanEvent()
{
	vkDestroyEvent(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

VkAccessFlags VulkanPipelineBarrier::AccessFlags(VkImageLayout Layout)
{
	VkAccessFlags Flags = 0u;

	switch (Layout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
	case VK_IMAGE_LAYOUT_GENERAL:
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		Flags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
#if VK_KHR_separate_depth_stencil_layouts
	case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
#endif
		Flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
#if VK_KHR_separate_depth_stencil_layouts
	case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
	case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
#endif
		Flags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		break;
#if VK_KHR_maintenance2
	case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
		Flags |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;
#endif
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		Flags |= VK_ACCESS_SHADER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		Flags |= VK_ACCESS_TRANSFER_READ_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		Flags |= VK_ACCESS_TRANSFER_WRITE_BIT;
		break;
#if VK_EXT_fragment_density_map
	case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
		Flags |= VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
		break;
#endif
#if VK_KHR_fragment_shading_rate
	case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
		Flags |= VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
		break;
#endif
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
	case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
	case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR:
	case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR:
		assert(0);
		break;
	}

	return Flags;
}

VkPipelineStageFlags VulkanPipelineBarrier::PipelineStageMask(VkImageLayout Layout)
{
	VkPipelineStageFlags Flags = 0u;

	switch (Layout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
	case VK_IMAGE_LAYOUT_GENERAL:
		Flags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		break;
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		Flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
#if VK_KHR_separate_depth_stencil_layouts
	case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL:
#endif
		Flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		break;
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
#if VK_KHR_maintenance2
	case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
	case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
#endif
#if VK_KHR_separate_depth_stencil_layouts
	case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL:
	case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL:
#endif
		Flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		break;
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		Flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		break;
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		Flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		break;
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		Flags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		break;
#if VK_EXT_fragment_density_map
	case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
		Flags |= VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
		break;
#endif
#if VK_KHR_fragment_shading_rate
	case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR:
		Flags |= VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV;
		break;
#endif
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
	case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
	case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR:
	case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR:
		assert(0);
		break;
	}

	return Flags;
}

VkPipelineStageFlags VulkanPipelineBarrier::PipelineStageMask(VkAccessFlags AccessMask)
{
	/// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/man/html/VkAccessFlagBits.html
	static const VkPipelineStageFlags AllShaderStages = 
		VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV |
		VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV |
		VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR |
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
		VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
		VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
		VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

	VkPipelineStageFlags Ret = VK_PIPELINE_STAGE_NONE_KHR;

	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_INDIRECT_COMMAND_READ_BIT))
	{
		Ret |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_INDEX_READ_BIT) || EnumHasAnyFlags(AccessMask, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT))
	{
		Ret |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_UNIFORM_READ_BIT))
	{
		Ret |= AllShaderStages;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_SHADER_READ_BIT))
	{
		Ret |= AllShaderStages | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_SHADER_WRITE_BIT))
	{
		Ret |= AllShaderStages;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_INPUT_ATTACHMENT_READ_BIT))
	{
		Ret |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_COLOR_ATTACHMENT_READ_BIT) || EnumHasAnyFlags(AccessMask, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT))
	{
		Ret |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT) || EnumHasAnyFlags(AccessMask, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT))
	{
		Ret |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_TRANSFER_READ_BIT) || EnumHasAnyFlags(AccessMask, VK_ACCESS_TRANSFER_WRITE_BIT))
	{
		Ret |= VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_HOST_READ_BIT) || EnumHasAnyFlags(AccessMask, VK_ACCESS_HOST_WRITE_BIT))
	{
		Ret |= VK_PIPELINE_STAGE_HOST_BIT;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_MEMORY_READ_BIT) || EnumHasAnyFlags(AccessMask, VK_ACCESS_MEMORY_WRITE_BIT))
	{
		Ret |= VK_PIPELINE_STAGE_NONE_KHR;
	}
#if VK_EXT_blend_operation_advanced
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT))
	{
		Ret |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
#endif
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_COMMAND_PREPROCESS_READ_BIT_NV) || EnumHasAnyFlags(AccessMask, VK_ACCESS_COMMAND_PREPROCESS_WRITE_BIT_NV))
	{
		Ret |= VK_PIPELINE_STAGE_COMMAND_PREPROCESS_BIT_NV;
	}
#if VK_EXT_conditional_rendering
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT))
	{
		Ret |= VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
	}
#endif
#if VK_KHR_fragment_shading_rate
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR))
	{
		Ret |= VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
	}
#endif
#if VK_EXT_transform_feedback
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT) || EnumHasAnyFlags(AccessMask, VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT))
	{
		Ret |= VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT))
	{
		Ret |= VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT | VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
	}
#endif
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR))
	{
		Ret |= AllShaderStages | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
	}
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR))
	{
		Ret |= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
	}
#if VK_EXT_fragment_density_map
	if (EnumHasAnyFlags(AccessMask, VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT))
	{
		Ret |= VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
	}
#endif

	return Ret;
}

VulkanPipelineBarrier::VkResourceState VulkanPipelineBarrier::TranslateResourceState(EResourceState State)
{
	VkPipelineStageFlags PipelineStageMask = VK_PIPELINE_STAGE_NONE_KHR;
	VkAccessFlags AccessMask = VK_ACCESS_NONE_KHR;
	VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

	/// VK_IMAGE_LAYOUT_UNDEFINED specifies that the layout is unknown. Image memory cannot be transitioned into this layout. 
	/// This layout can be used as the initialLayout member of VkImageCreateInfo. 
	/// This layout can be used in place of the current image layout in a layout transition, 
	/// but doing so will cause the contents of the image¡¯s memory to be undefined.

	if (State == EResourceState::Common)
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	}
	if (EnumHasAnyFlags(State, EResourceState::VertexBuffer))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		AccessMask |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
	}
	if (EnumHasAnyFlags(State, EResourceState::UniformBuffer))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		AccessMask |= VK_ACCESS_UNIFORM_READ_BIT;
	}
	if (EnumHasAnyFlags(State, EResourceState::IndexBuffer))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		AccessMask |= VK_ACCESS_INDEX_READ_BIT;
	}
	if (EnumHasAnyFlags(State, EResourceState::RenderTarget))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		AccessMask |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		ImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		/// VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL must only be used as a color or resolve attachment in a VkFramebuffer. 
		/// This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT usage bit enabled.
	}
	if (EnumHasAnyFlags(State, EResourceState::UnorderedAccess))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		AccessMask |= VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
		ImageLayout = VK_IMAGE_LAYOUT_GENERAL;  
		/// VK_IMAGE_LAYOUT_GENERAL supports all types of device access.
	}
	if (EnumHasAnyFlags(State, EResourceState::DepthWrite))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		AccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		ImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		/// VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL specifies a layout for both the depth and stencil aspects of a depth/stencil format image allowing read and write access as a depth/stencil attachment. 
		/// It is equivalent to VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL and VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL.
	}
	if (EnumHasAnyFlags(State, EResourceState::DepthRead))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		AccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		ImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	}
	if (EnumHasAnyFlags(State, EResourceState::StreamOut))
	{
#if VK_EXT_transform_feedback
		PipelineStageMask |= VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
		AccessMask |= VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT;
#endif
	}
	if (EnumHasAnyFlags(State, EResourceState::IndirectArgument))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
		AccessMask |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
	}
	if (EnumHasAnyFlags(State, EResourceState::TransferDst) || EnumHasAnyFlags(State, EResourceState::ResolveDst))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		AccessMask |= VK_ACCESS_TRANSFER_WRITE_BIT;
		ImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		/// VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL must only be used as a destination image of a transfer command. 
		/// This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_TRANSFER_DST_BIT usage bit enabled.
	}
	if (EnumHasAnyFlags(State, EResourceState::TransferSrc) || EnumHasAnyFlags(State, EResourceState::ResolveSrc))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		AccessMask |= VK_ACCESS_TRANSFER_READ_BIT;
		ImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		/// VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL must only be used as a source image of a transfer command (see the definition of VK_PIPELINE_STAGE_TRANSFER_BIT). 
		/// This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_TRANSFER_SRC_BIT usage bit enabled.
	}
	if (EnumHasAnyFlags(State, EResourceState::AccelerationStructure))
	{
		assert(0);
	}
	if (EnumHasAnyFlags(State, EResourceState::ShadingRate))
	{
#if VK_KHR_fragment_shading_rate
		PipelineStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
		AccessMask |= VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
		ImageLayout = VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR;
		/// VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR must only be used as a fragment shading rate attachment or shading rate image. 
		/// This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR usage bit enabled.
#endif
	}
	if (EnumHasAnyFlags(State, EResourceState::ShaderResource))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		AccessMask |= VK_ACCESS_SHADER_READ_BIT;
		ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		/// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL specifies a layout allowing read-only access in a shader as a sampled image, 
		/// combined image/sampler, or input attachment. 
		/// This layout is valid only for image subresources of images created with the VK_IMAGE_USAGE_SAMPLED_BIT or VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT usage bits enabled.
	}
	if (EnumHasAnyFlags(State, EResourceState::Present))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		AccessMask |= VK_ACCESS_MEMORY_READ_BIT;
		ImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		/// VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR is valid only for shared presentable images, and must be used for any usage the image supports.
	}
	if (EnumHasAnyFlags(State, EResourceState::InputAttachment))
	{
		PipelineStageMask |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		AccessMask |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	}

	return VkResourceState(std::move(std::make_tuple(PipelineStageMask, AccessMask, ImageLayout)));
}

const char8_t* NameofVkImageLayout(VkImageLayout Layout)
{
	switch (Layout)
	{
		case VK_IMAGE_LAYOUT_UNDEFINED: return "VK_IMAGE_LAYOUT_UNDEFINED";
		case VK_IMAGE_LAYOUT_GENERAL: return "VK_IMAGE_LAYOUT_GENERAL";
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: return "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL";
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL: return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL: return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL";
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: return "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL";
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL: return "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL";
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: return "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL";
		case VK_IMAGE_LAYOUT_PREINITIALIZED: return "VK_IMAGE_LAYOUT_PREINITIALIZED";
		case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL: return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL";
		case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL: return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL";
		case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL: return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL";
		case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL: return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL";
		case VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL: return "VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL";
		case VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL: return "VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL";
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: return "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR";
		case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR: return "VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR";
		case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT: return "VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT";
		case VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR: return "VK_IMAGE_LAYOUT_FRAGMENT_SHADING_RATE_ATTACHMENT_OPTIMAL_KHR";
		case VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR: return "VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL_KHR";
		case VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR: return "VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL_KHR";
	}

	return "VK_IMAGE_LAYOUT_UNDEFINED";
}

VulkanPipelineBarrier& VulkanPipelineBarrier::TransitionResourceState(VulkanImage* Image, EResourceState Src, EResourceState Dst, const VkImageSubresourceRange& SubresourceRange)
{
	assert(Image);

	if (Src != Dst)
	{
		auto SrcState = TranslateResourceState(Src);
		auto DstState = TranslateResourceState(Dst);

		SrcStageFlags |= SrcState.StageFlags();
		DstStageFlags |= DstState.StageFlags();
		
		Image->CurrentState = Dst;
		Image->SetImageLayout(DstState.ImageLayout());

		ImageBarriers.emplace_back(
			VkImageMemoryBarrier
			{
				VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
				nullptr,
				SrcState.AccessFlags(),
				DstState.AccessFlags(),
				SrcState.ImageLayout(),
				DstState.ImageLayout(),
				VK_QUEUE_FAMILY_IGNORED,
				VK_QUEUE_FAMILY_IGNORED,
				Image->Get(),
				SubresourceRange
			}
		);

		if (m_Command->Device()->Options().SubmitBarriersIM)
		{
			///LOG_DEBUG("Transition \"{}\"'s image layout from {} to {}", image->debugName(), nameofVkImageLayout(srcState.imageLayout()), nameofVkImageLayout(dstState.imageLayout()));
			Commit();
		}
	}

	return *this;
}

VulkanPipelineBarrier& VulkanPipelineBarrier::TransitionResourceState(VulkanBuffer* Buffer, EResourceState Src, EResourceState Dst)
{
	assert(Buffer);

	if (Src != Dst)
	{
		auto SrcState = TranslateResourceState(Src);
		auto DstState = TranslateResourceState(Dst);

		GlobalMemoryBarrier.srcAccessMask |= SrcState.AccessFlags();
		GlobalMemoryBarrier.dstAccessMask |= DstState.AccessFlags();

		SrcStageFlags |= SrcState.StageFlags();
		DstStageFlags |= DstState.StageFlags();

		Buffer->CurrentState = Dst;

		if (m_Command->Device()->Options().SubmitBarriersIM)
		{
			Commit();
		}
	}

	return *this;
}

void VulkanPipelineBarrier::Commit()
{
	assert(m_Command->Get() != VK_NULL_HANDLE);

	const bool8_t HasMemoryBarrier = GlobalMemoryBarrier.srcAccessMask != VK_ACCESS_NONE_KHR || GlobalMemoryBarrier.dstAccessMask != VK_ACCESS_NONE_KHR;

	if (HasMemoryBarrier || ImageBarriers.size() > 0u || BufferBarriers.size() > 0u)
	{
#if true
		if (m_Command->IsInsideRenderpass())
		{
			m_Command->EndRenderPass();
		}
#endif

		assert(!m_Command->IsInsideRenderpass());

		vkCmdPipelineBarrier(
			m_Command->Get(),
			SrcStageFlags,
			DstStageFlags,
			0u,
			HasMemoryBarrier ? 1u : 0u,
			HasMemoryBarrier ? &GlobalMemoryBarrier : nullptr,
			static_cast<uint32_t>(BufferBarriers.size()),
			BufferBarriers.data(),
			static_cast<uint32_t>(ImageBarriers.size()),
			ImageBarriers.data()
		);

		SrcStageFlags = DstStageFlags = VK_PIPELINE_STAGE_NONE_KHR;
		GlobalMemoryBarrier.srcAccessMask = GlobalMemoryBarrier.dstAccessMask = VK_ACCESS_NONE_KHR;
		ImageBarriers.clear();
		BufferBarriers.clear();
	}
}

NAMESPACE_END(RHI)
