#include "RHI/Vulkan/VulkanCommandBuffer.h"
#include "RHI/Vulkan/VulkanCommandPool.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanBuffer.h"
#include "RHI/Vulkan/VulkanPipeline.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "RHI/Vulkan/VulkanBarrier.h"
#include "Engine/RHI/RHICommandListContext.h"
#include "Engine/Services/SpdLogService.h"

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice& Device, VulkanCommandPool& Pool, ERHICommandBufferLevel Level)
	: VkDeviceResource(Device)
	, RHICommandBuffer(Level)
	, m_Pool(Pool)
	, m_Fence(Device, false)
{
	/**************************************************************************************************************************************************
	Each command buffer is always in one of the following states:

	Initial:
		When a command buffer is allocated, it is in the initial state. Some commands are able to reset a command buffer,
		or a set of command buffers, back to this state from any of the executable, recording or invalid state.
		Command buffers in the initial state can only be moved to the recording state, or freed.

	Recording:
		vkBeginCommandBuffer changes the state of a command buffer from the initial state to the recording state.
		Once a command buffer is in the recording state, vkCmd* commands can be used to record to the command buffer.

	Executable:
		vkEndCommandBuffer ends the recording of a command buffer, and moves it from the recording state to the executable state.
		Executable command buffers can be submitted, reset, or recorded to another command buffer.

	Pending:
		Queue submission of a command buffer changes the state of a command buffer from the executable state to the pending state.
		Whilst in the pending state, applications must not attempt to modify the command buffer in any way - as the device may be processing the commands recorded to it.
		Once execution of a command buffer completes, the command buffer reverts back to either the executable state, or the invalid state if it was recorded with VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT.
		A synchronization command should be used to detect when this occurs.

	Invalid:
		Some operations, such as modifying or deleting a resource that was used in a command recorded to a command buffer, will transition the state of that command buffer into the invalid state.
		Command buffers in the invalid state can only be reset or freed.

		Resetting a command buffer is an operation that discards any previously recorded commands and puts a command buffer in the initial state.
		Resetting occurs as a result of vkResetCommandBuffer or vkResetCommandPool, or as part of vkBeginCommandBuffer (which additionally puts the command buffer in the recording state).

		Secondary command buffers can be recorded to a primary command buffer via vkCmdExecuteCommands. This partially ties the lifecycle of the two command buffers together - if the primary is submitted to a queue,
		both the primary and any secondaries recorded to it move to the pending state. Once execution of the primary completes, so does any secondary recorded within it,
		and once all executions of each command buffer complete, they move to the executable state. If a secondary moves to any other state whilst it is recorded to another command buffer,
		the primary moves to the invalid state. A primary moving to any other state does not affect the state of the secondary.
		Resetting or freeing a primary command buffer removes the linkage to any secondary command buffers that were recorded to it.
	*****************************************************************************************************************************************************************/
}

void VulkanCommandBuffer::Begin()
{
	/*****************************************************************************************************************************************************************
	VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT:
		specifies that each recording of the command buffer will only be submitted once, and the command buffer will be resetand recorded again between each submission.

	VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT :
		specifies that a secondary command buffer is considered to be entirely inside a render pass.If this is a primary command buffer, then this bit is ignored.

	VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT :
		specifies that a command buffer can be resubmitted to a queue while it is in the pending state, and recorded into multiple primary command buffers.
	******************************************************************************************************************************************************************/

	assert(IsReady());

	vk::CommandBufferBeginInfo BeginInfo;
	BeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	
	VERIFY_VK(GetNative().begin(&BeginInfo));

	SetStatus(EStatus::Recording);
}

void VulkanCommandBuffer::End()
{
	assert(IsRecording());

	//if (m_InsideRenderPass)
	//{
	//	EndRenderPass();
	//}

	GetNative().end();

	SetStatus(EStatus::Ended);
}

//void VulkanCommandBuffer::BeginRenderPass(IFrameBuffer* FrameBuffer)
//{
//	assert(m_State == EStatus::Recording && FrameBuffer && !m_InsideRenderPass);
//
//	auto VkFrameBuffer = static_cast<VulkanFramebuffer*>(FrameBuffer);
//	assert(VkFrameBuffer);
//
//	VkRenderPassBeginInfo BeginInfo
//	{
//		VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
//		nullptr,
//		VkFrameBuffer->RenderPass(),
//		VkFrameBuffer->Get(),
//		VkRect2D
//		{
//			VkOffset2D {0, 0},
//			VkExtent2D {VkFrameBuffer->Width(), VkFrameBuffer->Height()}
//		},
//		0u,
//		nullptr
//	};
//
//#if VK_KHR_create_renderpass2
//	if (m_Device->EnabledExtensions().RenderPass2)
//	{
//		VkSubpassBeginInfo SubpassInfo
//		{
//			VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO,
//			nullptr,
//			VK_SUBPASS_CONTENTS_INLINE
//		};
//		vkCmdBeginRenderPass2(m_Handle, &BeginInfo, &SubpassInfo);
//	}
//	else
//#endif
//	{
//		vkCmdBeginRenderPass(m_Handle, &BeginInfo, Level() == ECommandBufferLevel::Primary ? VK_SUBPASS_CONTENTS_INLINE : VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
//	}
//
//	m_InsideRenderPass = true;
//}
//
//void VulkanCommandBuffer::EndRenderPass()
//{
//	assert(m_State == EStatus::Recording && m_InsideRenderPass);
//
//	vkCmdEndRenderPass(m_Handle);
//
//	m_InsideRenderPass = false;
//}

void VulkanCommandBuffer::Reset()
{
	/// The command buffer can be in any state other than pending, and is moved into the initial state
	/// Any primary command buffer that is in the recording or executable state and has commandBuffer recorded into it, becomes invalid.
	/// VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT specifies that most or all memory resources currently owned by the command buffer should be returned to the parent command pool. 
	/// If this flag is not set, then the command buffer may hold onto memory resources and reuse them when recording commands.

	/// commandBuffer must have been allocated from a pool that was created with the VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
	assert(!IsEnded());

	GetNative().reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	SetStatus(EStatus::Initial);
}

void VulkanCommandBuffer::BeginDebugMarker(const char* Name, const Math::Color& MarkerColor)
{
	assert(IsRecording() && Name);

	if (VulkanRHI::GetExtConfigs().DebugUtils)
	{
		vk::DebugUtilsLabelEXT DebugUtilsLabel;
		DebugUtilsLabel.setColor({ MarkerColor.x, MarkerColor.y, MarkerColor.z, MarkerColor.w })
			.setPLabelName(Name);

		GetNative().beginDebugUtilsLabelEXT(&DebugUtilsLabel);
	}
	else if (VulkanRHI::GetExtConfigs().DebugMarker)
	{
		vk::DebugMarkerMarkerInfoEXT DebugMarkerInfo;
		DebugMarkerInfo.setColor({ MarkerColor.x, MarkerColor.y, MarkerColor.z, MarkerColor.w })
			.setPMarkerName(Name);

		GetNative().debugMarkerBeginEXT(&DebugMarkerInfo);
	}
}

void VulkanCommandBuffer::EndDebugMarker()
{
	assert(IsRecording());

	if (VulkanRHI::GetExtConfigs().DebugUtils)
	{
		GetNative().endDebugUtilsLabelEXT();
	}
	else if (VulkanRHI::GetExtConfigs().DebugMarker)
	{
		GetNative().debugMarkerEndEXT();
	}
}

void VulkanCommandBuffer::SetVertexBuffer(const RHIBuffer* Buffer, uint32_t StartSlot, size_t Offset)
{
	assert(IsRecording());
	assert(StartSlot < GetDevice().GetPhysicalDeviceLimits().maxVertexInputBindings);

	GetNative().bindVertexBuffers(StartSlot, 1u, &Cast<VulkanBuffer>(Buffer)->GetNative(), &Offset);
}

void VulkanCommandBuffer::SetVertexStream(uint32_t StartSlot, const RHIVertexStream& VertexStream)
{
	assert(IsRecording());
	assert(StartSlot < GetDevice().GetPhysicalDeviceLimits().maxVertexInputBindings);

	std::vector<vk::Buffer> VertexBuffers(VertexStream.VertexBuffers.size());
	std::vector<vk::DeviceSize> Offsets(VertexStream.VertexBuffers.size());
	assert(VertexBuffers.size() < GetDevice().GetPhysicalDeviceLimits().maxVertexInputBindings);

	for (uint32_t Index = 0u; Index < VertexStream.VertexBuffers.size(); ++Index)
	{
		VertexBuffers[Index] = Cast<VulkanBuffer>(VertexStream.VertexBuffers[Index].Buffer)->GetNative();
		Offsets[Index] = VertexStream.VertexBuffers[Index].Offset;
	}

	GetNative().bindVertexBuffers(StartSlot, VertexBuffers, Offsets);
}

void VulkanCommandBuffer::SetIndexBuffer(const RHIBuffer* Buffer, size_t Offset, ERHIIndexFormat IndexFormat)
{
	assert(IsRecording() && Buffer);

	GetNative().bindIndexBuffer(Cast<VulkanBuffer>(Buffer)->GetNative(), Offset, GetIndexType(IndexFormat));
}

void VulkanCommandBuffer::SetPrimitiveTopology(ERHIPrimitiveTopology Topology)
{
	assert(IsRecording());

	if (VulkanRHI::GetExtConfigs().DynamicState)
	{
		GetNative().setPrimitiveTopologyEXT(GetPrimitiveTopology(Topology));
	}
}

void VulkanCommandBuffer::Draw(uint32_t VertexCount, uint32_t FirstVertex)
{
	assert(IsRecording());
	GetNative().draw(VertexCount, 1u, FirstVertex, 0u);
}

void VulkanCommandBuffer::DrawInstanced(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance)
{
	assert(IsRecording());
	GetNative().draw(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void VulkanCommandBuffer::DrawIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride)
{
	assert(IsRecording());
	GetNative().drawIndirect(Cast<VulkanBuffer>(IndirectBuffer)->GetNative(), Offset, DrawCount, Stride);
}

void VulkanCommandBuffer::DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, int32_t VertexOffset)
{
	assert(IsRecording());
	GetNative().drawIndexed(IndexCount, 1, FirstIndex, VertexOffset, 0u);
}

void VulkanCommandBuffer::DrawIndexedInstanced(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, int32_t VertexOffset, uint32_t FirstInstance)
{
	assert(IsRecording());
	GetNative().drawIndexed(IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}

void VulkanCommandBuffer::DrawIndexedIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride)
{
	assert(IsRecording());
	GetNative().drawIndexedIndirect(Cast<VulkanBuffer>(IndirectBuffer)->GetNative(), Offset, DrawCount, Stride);
}

void VulkanCommandBuffer::Dispatch(uint32_t GroupX, uint32_t GroupY, uint32_t GroupZ)
{
	assert(IsRecording());
	GetNative().dispatch(GroupX, GroupY, GroupZ);
}

void VulkanCommandBuffer::DispatchIndirect(const RHIBuffer* IndirectBuffer, size_t Offset)
{
	assert(IsRecording() && IndirectBuffer);
	GetNative().dispatchIndirect(Cast<VulkanBuffer>(IndirectBuffer)->GetNative(), Offset);
}

void VulkanCommandBuffer::SetViewport(const RHIViewport& Viewport)
{
	assert(IsRecording());

	vk::Viewport ViewportInfo;
	ViewportInfo.setX(Viewport.LeftTop.x)
		.setY(Viewport.LeftTop.y)
		.setWidth(Viewport.Extent.x)
		.setHeight(Viewport.Extent.y)
		.setMinDepth(Viewport.DepthRange.x)
		.setMaxDepth(Viewport.DepthRange.y);

	GetNative().setViewport(0u, 1u, &ViewportInfo);
}

void VulkanCommandBuffer::SetViewports(const RHIViewport* Viewports, uint32_t NumViewports)
{
	assert(IsRecording() && Viewports);

	std::vector<vk::Viewport> ViewportInfos(NumViewports);
	for (uint32_t Index = 0u; Index < NumViewports; ++Index)
	{
		auto& Viewport = Viewports[Index];

		ViewportInfos[Index].setX(Viewport.LeftTop.x)
			.setY(Viewport.LeftTop.y)
			.setWidth(Viewport.Extent.x)
			.setHeight(Viewport.Extent.y)
			.setMinDepth(Viewport.DepthRange.x)
			.setMaxDepth(Viewport.DepthRange.y);
	}

	GetNative().setViewport(0u, ViewportInfos);
}

void VulkanCommandBuffer::SetScissorRect(const RHIScissorRect& ScissorRect)
{
	assert(IsRecording());

	vk::Rect2D Rect;
	Rect.setExtent(vk::Extent2D(ScissorRect.Extent.x, ScissorRect.Extent.y))
		.setOffset(vk::Offset2D(ScissorRect.LeftTop.x, ScissorRect.LeftTop.y));

	GetNative().setScissor(0u, 1u, &Rect);
}

void VulkanCommandBuffer::SetScissorRects(const RHIScissorRect* ScissorRects, uint32_t NumScissorRects)
{
	assert(IsRecording() && ScissorRects);

	std::vector<vk::Rect2D> Rects(NumScissorRects);
	for (uint32_t Index = 0u; Index < NumScissorRects; ++Index)
	{
		auto& ScissorRect = ScissorRects[Index];

		Rects[Index].setExtent(vk::Extent2D(ScissorRect.Extent.x, ScissorRect.Extent.y))
			.setOffset(vk::Offset2D(ScissorRect.LeftTop.x, ScissorRect.LeftTop.y));
	}

	GetNative().setScissor(0u, Rects);
}

void VulkanCommandBuffer::RefreshStatus()
{
	if (IsSubmitted() && m_Fence.IsSignaled())
	{
		m_Fence.Reset();
		m_FenceSignaledCounter.fetch_add(1u);
		SetStatus(EStatus::NeedReset);
	}
}

void VulkanCommandBuffer::PushConstants(ERHIShaderStage Stage, const RHIBuffer* Buffer, const void* Data, size_t Size, size_t Offset)
{
	assert(IsRecording() && Buffer && Data && Size && Size < GetDevice().GetPhysicalDeviceLimits().maxPushConstantsSize);

	//(void)ConstantsBuffer;

	//assert(m_State == EStatus::Recording && Data && Size && Size <= m_Device->PhysicalLimits().maxPushConstantsSize && m_CurGfxPipeline);

	//vkCmdPushConstants(
	//	m_Handle, 
	//	m_CurGfxPipeline->Descriptor().PipelineLayout,
	//	VkType::ShaderStage(Stage) | VK_SHADER_STAGE_VERTEX_BIT,
	//	static_cast<uint32_t>(Offset), 
	//	static_cast<uint32_t>(Size), 
	//	Data);
}

void VulkanCommandBuffer::SetGraphicsPipeline(const RHIGraphicsPipeline* GraphicsPipeline)
{
	assert(IsRecording() && GraphicsPipeline);

	auto VulkanGfxPipeline = Cast<VulkanGraphicsPipeline>(GraphicsPipeline);
	assert(VulkanGfxPipeline);

//	assert(VkPipeline);
//
//	if (m_CurGfxPipeline != VkPipeline)
//	{
//		m_CurGfxPipeline = VkPipeline;
//#if true
//		if (m_InsideRenderPass)
//		{
//			EndRenderPass();
//		}
//#endif
}
//
//	auto VkState = static_cast<VulkanGraphicsPipelinEStatus*>(VkPipeline->State());
//	assert(VkState);
//
//	EnsurePipelineResourcEStatuss(VkState);
//
//	BeginRenderPass(VkState->FrameBuffer);
//
//	vkCmdBindPipeline(m_Handle, VK_PIPELINE_BIND_POINT_GRAPHICS, VkPipeline->Get());
//
//	vkCmdBindDescriptorSets(
//		m_Handle,
//		VK_PIPELINE_BIND_POINT_GRAPHICS,
//		VkPipeline->Descriptor().PipelineLayout,
//		0u,
//		1u,
//		&VkPipeline->Descriptor().DescriptorSet,
//		0u,
//		nullptr);
//
//	if (VkState->IsDirty<PipelinEStatus::EDirtyFlags::Resources>())
//	{
//		vkUpdateDescriptorSets(
//			m_Device->Get(),
//			static_cast<uint32_t>(VkState->WriteDescriptorSets().size()),
//			VkState->WriteDescriptorSets().data(),
//			0u,
//			nullptr);
//	}
//
//	/// if (VkState->IsDirty<PipelinEStatus::EDirtyFlags::VertexBuffer>() && VkState->VertexBuffer)
//	{
//		if (VkState->VertexBuffer)
//		{
//			VkBuffer Buffers[]{ static_cast<VulkanBuffer*>(VkState->VertexBuffer)->Get() };
//			VkDeviceSize Offsets[]{ 0u };
//			vkCmdBindVertexBuffers(m_Handle, 0u, 1u, Buffers, Offsets);
//		}
//	}
//
//	/// if (VkState->IsDirty<PipelinEStatus::EDirtyFlags::IndexBuffer>() && VkState->IndexBuffer.first)
//	{
//		if (VkState->IndexBuffer.first)
//		{
//			vkCmdBindIndexBuffer(
//				m_Handle,
//				static_cast<VulkanBuffer*>(VkState->IndexBuffer.first)->Get(),
//				0u,
//				VkState->IndexBuffer.second == EIndexFormat::UInt32 ? VK_INDEX_TYPE_UINT32 : VK_INDEX_TYPE_UINT16);
//		}
//	}
//
//	/// if (VkState->IsDirty<PipelinEStatus::EDirtyFlags::Viewport>())
//	{
//		std::vector<VkViewport> Viewports
//		{
//			VkViewport
//			{
//				VkState->Viewport.LeftTop.x,
//				m_Device->EnabledExtensions().Maintenance1 ? VkState->Viewport.Extent.y : VkState->Viewport.LeftTop.y,
//				VkState->Viewport.Extent.x,
//				m_Device->EnabledExtensions().Maintenance1 ? -VkState->Viewport.Extent.y : VkState->Viewport.Extent.y,
//				VkState->Viewport.Z.x,
//				VkState->Viewport.Z.y
//			}
//		};
//		vkCmdSetViewport(m_Handle, 0u, 1u, Viewports.data());
//	}
//
//	/// if (VkState->IsDirty<PipelinEStatus::EDirtyFlags::ScissorRect>())
//	{
//		std::vector<VkRect2D> ScissorRects
//		{
//			VkRect2D
//			{
//				VkOffset2D
//				{
//					static_cast<int32_t>(VkState->Scissor.LeftTop.x),
//					static_cast<int32_t>(VkState->Scissor.LeftTop.y)
//				},
//				VkExtent2D
//				{
//					static_cast<uint32_t>(VkState->Scissor.Extent.x),
//					static_cast<uint32_t>(VkState->Scissor.Extent.y)
//				}
//			}
//		};
//		vkCmdSetScissor(m_Handle, 0u, 1u, ScissorRects.data());
//	}
//
//	if (VkState->IsDirty<PipelinEStatus::EDirtyFlags::PolygonMode>())
//	{
//		SetPolygonMode(VkState->PolygonMode);
//	}
//
//	VkState->Reset();
//}

void VulkanCommandBuffer::ClearColorTexture(const RHITexture* Texture, const Math::Color& ClearColor)
{
	assert(!IsInsideRenderPass() && Texture);

//#if true
//	if (m_InsideRenderPass)
//	{
//		EndRenderPass();
//	}
//#endif
//
//	assert(DstImage && !m_InsideRenderPass);
//
//	auto VkImage = static_cast<VulkanImage*>(DstImage);
//
//	m_Tracker.Images.insert(VkImage);
//
//	VkImageSubresourceRange SubresourceRange
//	{
//		VK_IMAGE_ASPECT_COLOR_BIT,
//		0u,
//		VK_REMAINING_MIP_LEVELS,
//		0u,
//		VK_REMAINING_ARRAY_LAYERS
//	};
//	m_Barrier->TransitionResourcEStatus(VkImage, VkImage->CurrentState, EResourcEStatus::TransferDst, SubresourceRange);
//
//	VkClearColorValue ClearColorValue
//	{
//		ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w
//	};
//	vkCmdClearColorImage(m_Handle, VkImage->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearColorValue, 1u, &SubresourceRange);
//
//	m_Barrier->TransitionResourcEStatus(VkImage, EResourcEStatus::TransferDst, VkImage->RequiredState, SubresourceRange);
}

void VulkanCommandBuffer::ClearDepthStencilTexture(const RHITexture* Texture, bool ClearDepth, bool ClearStencil, float Depth, uint8_t Stencil)
{
	assert(!IsInsideRenderPass() && Texture);
//#if true
//	if (m_InsideRenderPass)
//	{
//		EndRenderPass();
//	}
//#endif
//
//	assert(DstImage && !m_InsideRenderPass);
//
//	if (ClearDepth || ClearStencil)
//	{
//		auto VkImage = static_cast<VulkanImage*>(DstImage);
//
//		m_Tracker.Images.insert(VkImage);
//
//		VkImageAspectFlags AspectFlags = 0u;
//		if (ClearDepth)
//		{
//			AspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
//		}
//		if (ClearStencil)
//		{
//			AspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
//		}
//
//		VkImageSubresourceRange SubresourceRange
//		{
//			AspectFlags,
//			0u,
//			VK_REMAINING_MIP_LEVELS,
//			0u,
//			VK_REMAINING_ARRAY_LAYERS
//		};
//		m_Barrier->TransitionResourcEStatus(VkImage, VkImage->CurrentState, EResourcEStatus::TransferDst, SubresourceRange);
//
//		VkClearDepthStencilValue ClearDepthStencilValue
//		{
//			Depth,
//			Stencil
//		};
//		vkCmdClearDepthStencilImage(m_Handle, VkImage->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearDepthStencilValue, 1u, &SubresourceRange);
//
//		m_Barrier->TransitionResourcEStatus(VkImage, EResourcEStatus::TransferDst, VkImage->RequiredState, SubresourceRange);
//	}
}

void VulkanCommandBuffer::WriteBuffer(const RHIBuffer* Buffer, const RHIBuffer* StagingBuffer, size_t Size, size_t SrcOffset, size_t DstOffset)
{
	assert(Buffer && StagingBuffer && Size && SrcOffset < Size && Size < Buffer->GetSize() && DstOffset < Size);
	assert(!IsInsideRenderPass());

	auto SrcBuffer = Cast<VulkanBuffer>(StagingBuffer);
	assert(SrcBuffer);

	auto DstBuffer = Cast<VulkanBuffer>(Buffer);
	assert(DstBuffer);

	vk::BufferCopy CopyRegion(SrcOffset, DstOffset, Size);
	GetNative().copyBuffer(SrcBuffer->GetNative(), DstBuffer->GetNative(), 1u, &CopyRegion);
}

void VulkanCommandBuffer::WriteTexture(const RHITexture* Texture, const RHIBuffer* StagingBuffer, size_t Size, uint32_t ArrayLayer, uint32_t MipLevel, size_t SrcOffset)
{
	assert(Texture && StagingBuffer && Size && SrcOffset < Size);
	assert(!IsInsideRenderPass());

	auto SrcBuffer = Cast<VulkanBuffer>(StagingBuffer);
	assert(SrcBuffer);

	auto DstImage = Cast<VulkanTexture>(Texture);
	assert(DstImage);

	uint32_t MipWidth = std::max(Texture->GetWidth() >> MipLevel, 1u);
	uint32_t MipHeight = std::max(Texture->GetHeight() >> MipLevel, 1u);
	uint32_t MipDepth = std::max(Texture->GetDepth() >> MipLevel, 1u);
	auto FormatAttributes = RHI::GetFormatAttributes(MipWidth, MipHeight, Texture->GetFormat());

	vk::ImageSubresourceRange SubresourceRange(vk::ImageAspectFlagBits::eColor, MipLevel, 1u, ArrayLayer, 1u);
	ERHIDeviceQueue DstQueue = VulkanRHI::GetConfigs().UseTransferQueue ? ERHIDeviceQueue::Transfer : ERHIDeviceQueue::Graphics;

	VulkanPipelineBarrier Barrier(GetDevice());
	Barrier.AddImageLayoutTransition(ERHIDeviceQueue::Graphics, DstQueue, DstImage->GetNative(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, SubresourceRange)
		.Submit(this);

	vk::BufferImageCopy CopyRegion(
		0u,
		FormatAttributes.NumCols * FormatAttributes.BlockSize,
		FormatAttributes.NumRows * FormatAttributes.BlockSize,
		vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, MipLevel, ArrayLayer, 1u),
		vk::Offset3D(0, 0, 0),
		vk::Extent3D(MipWidth, MipHeight, MipDepth));
	GetNative().copyBufferToImage(SrcBuffer->GetNative(), DstImage->GetNative(), vk::ImageLayout::eTransferDstOptimal, 1u, &CopyRegion);

	Barrier.AddImageLayoutTransition(ERHIDeviceQueue::Graphics, DstQueue, DstImage->GetNative(), vk::ImageLayout::eTransferDstOptimal, ::GetImageLayout(Texture->GetState()), SubresourceRange)
		.Submit(this);
}

void VulkanCommandBuffer::WriteTexture(const RHITexture* Texture, const RHIBuffer* StagingBuffer, size_t Size, size_t SrcOffset)
{
	assert(Texture && StagingBuffer && Size && SrcOffset < Size);
	assert(!IsInsideRenderPass());

	auto SrcBuffer = Cast<VulkanBuffer>(StagingBuffer);
	assert(SrcBuffer);

	auto DstImage = Cast<VulkanTexture>(Texture);
	assert(DstImage);

	std::vector<vk::BufferImageCopy> CopyRegions;
	CopyRegions.reserve(Texture->GetNumArrayLayer() * Texture->GetNumMipLevel());

	for (uint32_t ArrayLayer = 0u; ArrayLayer < Texture->GetNumArrayLayer(); ++ArrayLayer)
	{
		for (uint32_t MipLevel = 0u; MipLevel < Texture->GetNumMipLevel(); ++MipLevel)
		{
			uint32_t MipWidth = std::max(Texture->GetWidth() >> MipLevel, 1u);
			uint32_t MipHeight = std::max(Texture->GetHeight() >> MipLevel, 1u);
			uint32_t MipDepth = std::max(Texture->GetDepth() >> MipLevel, 1u);
			auto FormatAttributes = RHI::GetFormatAttributes(MipWidth, MipHeight, Texture->GetFormat());

			CopyRegions.emplace_back(vk::BufferImageCopy(
				0u,
				FormatAttributes.NumCols * FormatAttributes.BlockSize,
				FormatAttributes.NumRows * FormatAttributes.BlockSize,
				vk::ImageSubresourceLayers(vk::ImageAspectFlagBits::eColor, MipLevel, ArrayLayer, 1u),
				vk::Offset3D(0, 0, 0),
				vk::Extent3D(MipWidth, MipHeight, MipDepth)));
		}
	}

	vk::ImageSubresourceRange SubresourceRange(vk::ImageAspectFlagBits::eColor, 0u, VK_REMAINING_MIP_LEVELS, 0u, VK_REMAINING_ARRAY_LAYERS);
	ERHIDeviceQueue DstQueue = VulkanRHI::GetConfigs().UseTransferQueue ? ERHIDeviceQueue::Transfer : ERHIDeviceQueue::Graphics;

	VulkanPipelineBarrier Barrier(GetDevice());
	Barrier.AddImageLayoutTransition(ERHIDeviceQueue::Graphics, DstQueue, DstImage->GetNative(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, SubresourceRange)
		.Submit(this);

	GetNative().copyBufferToImage(SrcBuffer->GetNative(), DstImage->GetNative(), vk::ImageLayout::eTransferDstOptimal, CopyRegions);

	Barrier.AddImageLayoutTransition(ERHIDeviceQueue::Graphics, DstQueue, DstImage->GetNative(), vk::ImageLayout::eTransferDstOptimal, ::GetImageLayout(Texture->GetState()), SubresourceRange)
		.Submit(this);
}

//void VulkanCommandBuffer::SetPolygonMode(EPolygonMode Mode)
//{
//#if VK_EXT_extended_dynamic_state
//	if (m_Device->EnabledExtensions().ExtendedDynamicState)
//	{
//		assert(m_State == EStatus::Recording);
//
//		vkCmdSetCullModeEXT(m_Handle, VkType::PolygonMode(Mode));
//	}
//	else
//	{
//		assert(false);
//	}
//#else
//	(void)Mode;
//#endif
//}

//void VulkanCommandBuffer::ExecuteSecondaryCommands(const std::vector<std::shared_ptr<VulkanCommandBuffer>>& Commands)
//{
//	/*********************************************************************************************************************
//	If any element of pCommandBuffers was not recorded with the VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT flag,
//		and it was recorded into any other primary command buffer, that primary command buffer must not be in the pending state
//
//		it must not be in the pending state.
//
//		it must not have already been recorded to commandBuffer.
//
//		it must not appear more than once in pCommandBuffers.
//
//	Each element of pCommandBuffers must have been allocated from a VkCommandPool that was created for the same queue family as the VkCommandPool from which commandBuffer was allocated
//
//	If vkCmdExecuteCommands is being called within a render pass instance,
//		that render pass instance must have been begun with the contents parameter of vkCmdBeginRenderPass set to VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
//
//		each element of pCommandBuffers must have been recorded with the VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
//
//		each element of pCommandBuffers must have been recorded with VkCommandBufferInheritanceInfo::subpass set to the index of the subpass which the given command buffer will be executed in
//
//		the render passes specified in the pBeginInfo->pInheritanceInfo->renderPass members of the vkBeginCommandBuffer commands used to begin recording each element of pCommandBuffers must be compatible with the current render pass.
//
//		and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance
//
//	If vkCmdExecuteCommands is not being called within a render pass instance, each element of pCommandBuffers must not have been recorded with the VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
//	*************************************************************************************************************************/
//
//	assert(Level() == ECommandBufferLevel::Primary && Commands.size() > 0u && m_State == EStatus::Recording);
//
//	std::vector<VkCommandBuffer> VkCommands;
//
//	std::transform(Commands.begin(), Commands.end(), VkCommands.begin(), [](std::shared_ptr<VulkanCommandBuffer> Command) {
//		assert(Command->Level() == ECommandBufferLevel::Secondary && Command->State() == EStatus::Pending || Command->State() == EStatus::Executable);
//		return Command->Get();
//		});
//
//	vkCmdExecuteCommands(m_Handle, static_cast<uint32_t>(VkCommands.size()), VkCommands.data());
//}

//void VulkanCommandBuffer::EnsurePipelineResourcEStatuss(VulkanGraphicsPipelinEStatus* State)
//{
//	assert(State && State->FrameBuffer);
//
//	for (auto& Image : State->FrameBuffer->Description().ColorAttachments)
//	{
//		auto VkImage = static_cast<VulkanImage*>(Image.get());
//		VkImage->RequiredState = EResourcEStatus::RenderTarget;
//
//		m_Tracker.Images.insert(VkImage);
//
//		if (VkImage->PermanentState != EResourcEStatus::Unknown)
//		{
//			m_Tracker.PermanentImages.insert(VkImage);
//		}
//	}
//
//	if (State->FrameBuffer->Description().DepthStencilAttachment)
//	{
//		auto VkImage = static_cast<VulkanImage*>(State->FrameBuffer->Description().DepthStencilAttachment.get());
//		VkImage->RequiredState = EResourcEStatus::DepthWrite;
//
//		m_Tracker.Images.insert(VkImage);
//	}
//
//	for (auto& Resource : State->ResourcesNeedTransitionState)
//	{
//		if (Resource.Buffer)
//		{
//			Resource.Buffer->RequiredState = EResourcEStatus::UniformBuffer;
//			m_Tracker.Buffers.insert(Resource.Buffer);
//		}
//
//		if (Resource.Image)
//		{
//			Resource.Image->RequiredState = EResourcEStatus::ShaderResource;
//			m_Tracker.Images.insert(Resource.Image);
//		}
//	}
//
//	m_Tracker.EnsureCorrectStates(m_Barrier.get());
//}
//
//void VulkanCommandBuffer::VkResourcEStatusTracker::EnsureCorrectStates(VulkanPipelineBarrier* Barrier)
//{
//	for (auto Image : Images)
//	{
//		assert(Image);
//
//		if (Image->CurrentState != Image->RequiredState)
//		{
//			VkImageSubresourceRange SubresourceRange
//			{
//				Image->VkAttributes().Aspect,
//				0u,
//				VK_REMAINING_MIP_LEVELS,
//				0u,
//				VK_REMAINING_ARRAY_LAYERS
//			};
//
//			Barrier->TransitionResourcEStatus(Image, Image->CurrentState, Image->RequiredState, SubresourceRange);
//		}
//	}
//	Images.clear();
//
//	for (auto Buffer : Buffers)
//	{
//		assert(Buffer);
//
//		if (Buffer->CurrentState != Buffer->RequiredState)
//		{
//			Barrier->TransitionResourcEStatus(Buffer, Buffer->CurrentState, Buffer->RequiredState);
//		}
//	}
//	Buffers.clear();
//}
//
//NAMESPACE_END(RHI)
//
//#endif
