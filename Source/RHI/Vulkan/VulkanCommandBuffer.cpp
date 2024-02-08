#include "RHI/Vulkan/VulkanCommandBuffer.h"
#include "RHI/Vulkan/VulkanCommandPool.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanRHI.h"
#include "RHI/Vulkan/VulkanBuffer.h"
#include "RHI/Vulkan/VulkanLayerExtensions.h"
#include "Engine/Services/SpdLogService.h"

VulkanCommandBuffer::VulkanCommandBuffer(const VulkanDevice& Device, VulkanCommandPool& Pool, ERHICommandBufferLevel Level)
	: VkDeviceResource(Device)
	, RHICommandBuffer(Level)
	, m_Pool(Pool)
	, m_Semaphore(Device)
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

	assert(m_State == EState::Initial);

	auto vkBeginInfo = vk::CommandBufferBeginInfo()
		.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	
	VERIFY_VK(GetNative().begin(&vkBeginInfo));

	SetState(EState::Recording);
}

void VulkanCommandBuffer::End()
{
	assert(m_State == EState::Recording);

	//m_Tracker.EnsureCorrectStates(m_Barrier.get());

	//for (auto Image : m_Tracker.PermanentImages)
	//{
	//	VkImageSubresourceRange SubresourceRange
	//	{
	//		VK_IMAGE_ASPECT_COLOR_BIT,
	//		0u,
	//		VK_REMAINING_MIP_LEVELS,
	//		0u,
	//		VK_REMAINING_ARRAY_LAYERS
	//	};
	//	m_Barrier->TransitionResourceState(Image, Image->CurrentState, Image->PermanentState, SubresourceRange);
	//}
	//m_Tracker.PermanentImages.clear();

	//m_Barrier->Commit();

	//if (m_InsideRenderPass)
	//{
	//	EndRenderPass();
	//}

	//VERIFY_VK(vkEndCommandBuffer(Get()));

	SetState(EState::Executable);
}

//void VulkanCommandBuffer::BeginRenderPass(IFrameBuffer* FrameBuffer)
//{
//	assert(m_State == EState::Recording && FrameBuffer && !m_InsideRenderPass);
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
//	assert(m_State == EState::Recording && m_InsideRenderPass);
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
	assert(m_State != EState::Executable);

	GetNative().reset(vk::CommandBufferResetFlagBits::eReleaseResources);

	SetState(EState::Initial);
}

void VulkanCommandBuffer::BeginDebugMarker(const char* Name, const Math::Color& MarkerColor)
{
	assert(m_State == EState::Recording && Name);

	if (VulkanRHI::GetLayerExtensionConfigs().HasDebugUtilsExt)
	{
		
	}
	else if (VulkanRHI::GetLayerExtensionConfigs().HasDebugMarkerExt)
	{
		auto vkDebugMarkerInfo = vk::DebugMarkerMarkerInfoEXT()
			.setColor({ MarkerColor.x, MarkerColor.y, MarkerColor.z, MarkerColor.w })
			.setPMarkerName(Name);
		GetNative().debugMarkerBeginEXT(&vkDebugMarkerInfo);
	}
}

void VulkanCommandBuffer::EndDebugMarker()
{
	assert(m_State == EState::Recording);

	if (VulkanRHI::GetLayerExtensionConfigs().HasDebugUtilsExt)
	{

	}
	else if (VulkanRHI::GetLayerExtensionConfigs().HasDebugMarkerExt)
	{
		GetNative().debugMarkerEndEXT();
	}
}

void VulkanCommandBuffer::SetVertexBuffer(const RHIBuffer* Buffer, uint32_t StartSlot, size_t Offset)
{
	assert(m_State == EState::Recording);

	GetNative().bindVertexBuffers(StartSlot, 1u, &Cast<VulkanBuffer>(Buffer)->GetNative(), &Offset);
}

void VulkanCommandBuffer::SetIndexBuffer(const RHIBuffer* Buffer, size_t Offset, ERHIIndexFormat Format)
{
	assert(m_State == EState::Recording);

	GetNative().bindIndexBuffer(Cast<VulkanBuffer>(Buffer)->GetNative(), Offset, Format == ERHIIndexFormat::UInt32 ? vk::IndexType::eUint32 : vk::IndexType::eUint16);
}

void VulkanCommandBuffer::SetPrimitiveTopology(ERHIPrimitiveTopology Topology)
{
	assert(m_State == EState::Recording);

#if VK_VERSION_1_3
	GetNative().setPrimitiveTopology(GetPrimitiveTopology(Topology));
#else
	if (VulkanRHI::GetLayerExtensionConfigs().HasDynamicState)
	{
		GetNative().setPrimitiveTopologyEXT(GetPrimitiveTopology(Topology));
	}
#endif
}

void VulkanCommandBuffer::Draw(uint32_t VertexCount, uint32_t FirstVertex)
{
	assert(m_State == EState::Recording);
	GetNative().draw(VertexCount, 1u, FirstVertex, 0u);
}

void VulkanCommandBuffer::DrawInstanced(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance)
{
	assert(m_State == EState::Recording);
	GetNative().draw(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void VulkanCommandBuffer::DrawIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride)
{
	assert(m_State == EState::Recording);
	GetNative().drawIndirect(Cast<VulkanBuffer>(IndirectBuffer)->GetNative(), Offset, DrawCount, Stride);
}

void VulkanCommandBuffer::DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, int32_t VertexOffset)
{
	assert(m_State == EState::Recording);
	GetNative().drawIndexed(IndexCount, 1, FirstIndex, VertexOffset, 0u);
}

void VulkanCommandBuffer::DrawIndexedInstanced(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, int32_t VertexOffset, uint32_t FirstInstance)
{
	assert(m_State == EState::Recording);
	GetNative().drawIndexed(IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}

void VulkanCommandBuffer::DrawIndexedIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride)
{
	assert(m_State == EState::Recording);
	GetNative().drawIndexedIndirect(Cast<VulkanBuffer>(IndirectBuffer)->GetNative(), Offset, DrawCount, Stride);
}

void VulkanCommandBuffer::Dispatch(uint32_t GroupX, uint32_t GroupY, uint32_t GroupZ)
{
	assert(m_State == EState::Recording);
	GetNative().dispatch(GroupX, GroupY, GroupZ);
}

void VulkanCommandBuffer::DispatchIndirect(const RHIBuffer* IndirectBuffer, size_t Offset)
{
	assert(m_State == EState::Recording && IndirectBuffer);
	GetNative().dispatchIndirect(Cast<VulkanBuffer>(IndirectBuffer)->GetNative(), Offset);
}

//void VulkanCommandBuffer::CopyBufferToImage(const RHIImage* DstImage, const void* SrcBuffer, uint32_t BufferSize, const RHIImageSubresourceRange& SubresourceRange)
//{
//#if true
//	if (m_InsideRenderPass)
//	{
//		EndRenderPass();
//	}
//#endif
//
//	assert(!m_InsideRenderPass && m_State == EState::Recording && DstImage && SrcBuffer && BufferSize);
//
//	/// bufferRowLength and bufferImageHeight specify in texels a subregion of a larger two- or three-dimensional image in buffer memory, 
//	/// and control the addressing calculations. If either of these values is zero, 
//	/// that aspect of the buffer memory is considered to be tightly packed according to the imageExtent.
//	
//	size_t NumMipLevels = SubresourceRange.LevelCount;
//	size_t NumArrayLayers = SubresourceRange.LayerCount;
//	if (SubresourceRange.LevelCount == ImageSubresourceRange::AllMipLevels)
//	{
//		NumMipLevels = DstImage->MipLevels();
//	}
//	if (SubresourceRange.LayerCount == ImageSubresourceRange::AllArrayLayers)
//	{
//		NumArrayLayers = DstImage->ArrayLayers();
//	}
//
//	assert(SubresourceRange.BaseMipLevel < NumMipLevels&& SubresourceRange.BaseArrayLayer < NumArrayLayers);
//	assert(NumMipLevels > 0u && NumMipLevels < ImageSubresourceRange::AllMipLevels);
//	assert(NumArrayLayers > 0u && NumArrayLayers < ImageSubresourceRange::AllArrayLayers);
//
//	auto StagingHeap = StagingBufferAllocator::Get().Alloc(BufferSize); /// #TODO Memory waste when not all subresources ???
//	assert(StagingHeap->Size >= BufferSize);
//
//	size_t Offset = StagingHeap->CurOffset;
//	uint8_t* MappedAddress = reinterpret_cast<uint8_t*>(StagingHeap->Buffer->MappedMemory());
//	if (SubresourceRange == AllSubresource)
//	{
//		VERIFY(memcpy_s(MappedAddress + Offset, BufferSize, SrcBuffer, BufferSize) == 0);
//	}
//
//	auto VkImage = static_cast<VulkanImage*>(DstImage);
//
//	std::vector<VkBufferImageCopy> ImageCopys(NumMipLevels * NumArrayLayers);
//	for (uint32_t MipIndex = SubresourceRange.BaseMipLevel; MipIndex < NumMipLevels; ++MipIndex)
//	{
//		for (uint32_t ArrayIndex = SubresourceRange.BaseArrayLayer; ArrayIndex < NumArrayLayers; ++ArrayIndex)
//		{
//			/// size_t index = i * levels + j;
//			uint32_t MipWidth = std::max(DstImage->Width() >> DstImage->MipLevels(), 1u);
//			uint32_t MipHeight = std::max(DstImage->Height() >> DstImage->MipLevels(), 1u);
//			uint32_t MipDepth = std::max(DstImage->Depth() >> DstImage->MipLevels(), 1u);
//			uint32_t SliceBytes = 0u;
//			uint32_t RowBytes = 0u;
//			auto ColRowBytes = FormatAttribute::CalculateFormatBytes(
//				MipWidth,
//				MipHeight,
//				DstImage->Format(),
//				SliceBytes,
//				RowBytes);
//			
//			ImageCopys[MipIndex] = VkBufferImageCopy
//			{
//				static_cast<uint32_t>(Offset),
//				ColRowBytes.first,
//				ColRowBytes.second,
//				VkImageSubresourceLayers
//				{
//					VkImage->VkAttributes().Aspect,
//					MipIndex,
//					ArrayIndex,
//					1u
//				},
//				VkOffset3D {0, 0, 0},
//				VkExtent3D
//				{
//					MipWidth,
//					MipHeight,
//					MipDepth
//				}
//			};
//
//			uint32_t RowPitch = SliceBytes * MipDepth;
//			if (SubresourceRange != AllSubresource)
//			{
//				VERIFY(memcpy_s(MappedAddress + Offset, RowPitch, reinterpret_cast<const uint8_t*>(SrcBuffer) + RowPitch, RowPitch) == 0);
//			}
//
//			Offset += RowPitch;
//		}
//	}
//
//	VkImageSubresourceRange VkSubresourceRange
//	{
//		VkImage->VkAttributes().Aspect,
//		SubresourceRange.BaseMipLevel,
//		SubresourceRange.LevelCount == ImageSubresourceRange::AllMipLevels ? VK_REMAINING_MIP_LEVELS : SubresourceRange.LevelCount,
//		SubresourceRange.BaseArrayLayer,
//		SubresourceRange.LayerCount == ImageSubresourceRange::AllArrayLayers ? VK_REMAINING_ARRAY_LAYERS : SubresourceRange.LayerCount
//	};
//
//	m_Barrier->TransitionResourceState(VkImage, VkImage->CurrentState, EResourceState::TransferDst, VkSubresourceRange);
//
//	vkCmdCopyBufferToImage(
//		m_Handle,
//		std::static_pointer_cast<VulkanBuffer>(StagingHeap->Buffer)->Get(),
//		VkImage->Get(),
//		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//		static_cast<uint32_t>(ImageCopys.size()),
//		ImageCopys.data());
//
//	m_Barrier->TransitionResourceState(VkImage, EResourceState::TransferDst, VkImage->RequiredState, VkSubresourceRange);
//}

//void VulkanCommandBuffer::CopyBuffer(const RHIBuffer* DstBuffer, const void* Data, size_t DataSize, size_t SrcOffset, size_t DstOffset)
//{
//	/// https://github.com/KhronosGroup/Vulkan-Docs/wiki/Synchronization-Examples-(Legacy-synchronization-APIs)
//	/// Global memory barrier covers all resources. Generally considered more efficient to do a global memory barrier than per-resource barriers, 
//	/// per-resource barriers should usually be used for queue ownership transfers and image layout transitions - otherwise use global barriers.
//
//	assert(m_State == EState::Recording && DstBuffer && DataSize && Data);

//	if (DstBuffer->Update(Data, DataSize, SrcOffset, DstOffset))
//	{
//		return;
//	}
//
//#if true
//	if (m_InsideRenderPass)
//	{
//		EndRenderPass();
//	}
//#endif
//
//	auto VkBuffer = static_cast<VulkanBuffer*>(DstBuffer);
//
//	assert(!m_InsideRenderPass);
//
//	auto Heap = StagingBufferAllocator::Get().Alloc(DataSize);
//	uint8_t* MappedAddress = reinterpret_cast<uint8_t*>(Heap->Buffer->MappedMemory());
//	VERIFY(memcpy_s(MappedAddress + Heap->CurOffset, DataSize, Data, DataSize) == 0);
//
//	VkBufferCopy CopyRegion
//	{
//		SrcOffset + Heap->CurOffset,
//		DstOffset,
//		DataSize
//	};
//
//	m_Tracker.Buffers.insert(VkBuffer);
//
//	m_Barrier->TransitionResourceState(VkBuffer, VkBuffer->CurrentState, EResourceState::TransferDst);
//
//	vkCmdCopyBuffer(
//		m_Handle,
//		static_cast<VulkanBuffer*>(Heap->Buffer.get())->Get(),
//		VkBuffer->Get(),
//		1u,
//		&CopyRegion);
//
//	m_Barrier->TransitionResourceState(VkBuffer, EResourceState::TransferDst, VkBuffer->RequiredState);
//}

//void VulkanCommandBuffer::CopyImage(IImage* SrcImage, const ImageSlice& SrcSlice, IImage* DstImage, const ImageSlice& DstSlice)
//{
//#if true
//	if (m_InsideRenderPass)
//	{
//		EndRenderPass();
//	}
//#endif
//
//	/// https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#formats-compatibility
//	assert(m_State == EState::Recording && !m_InsideRenderPass && SrcImage && DstImage);
//
//	auto VkSrcImage = static_cast<VulkanImage*>(SrcImage);
//	auto VkDstImage = static_cast<VulkanImage*>(DstImage);
//
//	VkImageCopy ImageCopy;
//	ImageCopy.srcSubresource = VkImageSubresourceLayers
//	{
//		VkSrcImage->VkAttributes().Aspect,
//		0u,
//		0u,
//		VkSrcImage->ArrayLayers(),
//	};
//	ImageCopy.srcOffset = VkOffset3D{SrcSlice.X, SrcSlice.Y, SrcSlice.Z};
//	ImageCopy.dstSubresource = VkImageSubresourceLayers
//	{
//		VkDstImage->VkAttributes().Aspect,
//		0u,
//		0u,
//		VkDstImage->ArrayLayers(),
//	};
//	ImageCopy.dstOffset = VkOffset3D{DstSlice.X, DstSlice.Y, DstSlice.Z};
//	ImageCopy.extent = VkExtent3D{DstSlice.Width, DstSlice.Height, DstSlice.Depth};
//
//	VkImageSubresourceRange SrcSubresourceRange
//	{
//		VkSrcImage->VkAttributes().Aspect,
//		0u,
//		VK_REMAINING_MIP_LEVELS,
//		0u,
//		VK_REMAINING_ARRAY_LAYERS
//	};
//
//	VkImageSubresourceRange DstSubresourceRange
//	{
//		VkDstImage->VkAttributes().Aspect,
//		0u,
//		VK_REMAINING_MIP_LEVELS,
//		0u,
//		VK_REMAINING_ARRAY_LAYERS
//	};
//
//	m_Barrier->TransitionResourceState(VkSrcImage, VkSrcImage->CurrentState, EResourceState::TransferSrc, SrcSubresourceRange);
//	m_Barrier->TransitionResourceState(VkDstImage, VkDstImage->CurrentState, EResourceState::TransferDst, DstSubresourceRange);
//
//	vkCmdCopyImage(m_Handle, VkSrcImage->Get(), VkSrcImage->VkAttributes().Layout, VkDstImage->Get(), VkDstImage->VkAttributes().Layout, 1u, &ImageCopy);
//}

void VulkanCommandBuffer::SetViewport(const RHIViewport& Viewport)
{
	assert(m_State == EState::Recording);

	auto vkViewport = vk::Viewport()
		.setX(Viewport.LeftTop.x)
		.setY(Viewport.LeftTop.y)
		.setWidth(Viewport.Extent.x)
		.setHeight(Viewport.Extent.y)
		.setMinDepth(Viewport.DepthRange.x)
		.setMaxDepth(Viewport.DepthRange.y);
	GetNative().setViewport(0u, 1u, &vkViewport);
}

void VulkanCommandBuffer::SetViewports(const RHIViewport* Viewports, uint32_t NumViewports)
{
	assert(m_State == EState::Recording && Viewports);

	std::vector<vk::Viewport> vkViewports(NumViewports);
	for (uint32_t Index = 0u; Index < NumViewports; ++Index)
	{
		auto& Viewport = Viewports[Index];
		vkViewports[Index] = vk::Viewport()
			.setX(Viewport.LeftTop.x)
			.setY(Viewport.LeftTop.y)
			.setWidth(Viewport.Extent.x)
			.setHeight(Viewport.Extent.y)
			.setMinDepth(Viewport.DepthRange.x)
			.setMaxDepth(Viewport.DepthRange.y);
	}

	GetNative().setViewport(0u, vkViewports);
}

void VulkanCommandBuffer::SetScissorRect(const RHIScissorRect& ScissorRect)
{
	assert(m_State == EState::Recording);

	auto vkRect = vk::Rect2D()
		.setExtent(vk::Extent2D(ScissorRect.Extent.x, ScissorRect.Extent.y))
		.setOffset(vk::Offset2D(ScissorRect.LeftTop.x, ScissorRect.LeftTop.y));
	GetNative().setScissor(0u, 1u, &vkRect);
}

void VulkanCommandBuffer::SetScissorRects(const RHIScissorRect* ScissorRects, uint32_t NumScissorRects)
{
	assert(m_State == EState::Recording && ScissorRects);

	std::vector<vk::Rect2D> vkRects(NumScissorRects);
	for (uint32_t Index = 0u; Index < NumScissorRects; ++Index)
	{
		auto& ScissorRect = ScissorRects[Index];
		vkRects[Index] = vk::Rect2D()
			.setExtent(vk::Extent2D(ScissorRect.Extent.x, ScissorRect.Extent.y))
			.setOffset(vk::Offset2D(ScissorRect.LeftTop.x, ScissorRect.LeftTop.y));
	}
	GetNative().setScissor(0u, vkRects);
}

void VulkanCommandBuffer::PushConstants(ERHIShaderStage Stage, const RHIBuffer* ConstantsBuffer, const void* Data, size_t Size, size_t Offset)
{
	//(void)ConstantsBuffer;

	//assert(m_State == EState::Recording && Data && Size && Size <= m_Device->PhysicalLimits().maxPushConstantsSize && m_CurGfxPipeline);

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
//	assert(m_State == EState::Recording && DstPipeline);
//
//	auto VkPipeline = static_cast<VulkanGraphicsPipeline*>(DstPipeline);
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
//	auto VkState = static_cast<VulkanGraphicsPipelineState*>(VkPipeline->State());
//	assert(VkState);
//
//	EnsurePipelineResourceStates(VkState);
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
//	if (VkState->IsDirty<PipelineState::EDirtyFlags::Resources>())
//	{
//		vkUpdateDescriptorSets(
//			m_Device->Get(),
//			static_cast<uint32_t>(VkState->WriteDescriptorSets().size()),
//			VkState->WriteDescriptorSets().data(),
//			0u,
//			nullptr);
//	}
//
//	/// if (VkState->IsDirty<PipelineState::EDirtyFlags::VertexBuffer>() && VkState->VertexBuffer)
//	{
//		if (VkState->VertexBuffer)
//		{
//			VkBuffer Buffers[]{ static_cast<VulkanBuffer*>(VkState->VertexBuffer)->Get() };
//			VkDeviceSize Offsets[]{ 0u };
//			vkCmdBindVertexBuffers(m_Handle, 0u, 1u, Buffers, Offsets);
//		}
//	}
//
//	/// if (VkState->IsDirty<PipelineState::EDirtyFlags::IndexBuffer>() && VkState->IndexBuffer.first)
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
//	/// if (VkState->IsDirty<PipelineState::EDirtyFlags::Viewport>())
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
//	/// if (VkState->IsDirty<PipelineState::EDirtyFlags::ScissorRect>())
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
//	if (VkState->IsDirty<PipelineState::EDirtyFlags::PolygonMode>())
//	{
//		SetPolygonMode(VkState->PolygonMode);
//	}
//
//	VkState->Reset();
//}

void VulkanCommandBuffer::ClearColorImage(const RHIImage* DstImage, const Math::Color& ClearColor)
{
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
//	m_Barrier->TransitionResourceState(VkImage, VkImage->CurrentState, EResourceState::TransferDst, SubresourceRange);
//
//	VkClearColorValue ClearColorValue
//	{
//		ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w
//	};
//	vkCmdClearColorImage(m_Handle, VkImage->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearColorValue, 1u, &SubresourceRange);
//
//	m_Barrier->TransitionResourceState(VkImage, EResourceState::TransferDst, VkImage->RequiredState, SubresourceRange);
}

void VulkanCommandBuffer::ClearDepthStencilImage(const RHIImage* DstImage, bool ClearDepth, bool ClearStencil, float Depth, uint8_t Stencil)
{
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
//		m_Barrier->TransitionResourceState(VkImage, VkImage->CurrentState, EResourceState::TransferDst, SubresourceRange);
//
//		VkClearDepthStencilValue ClearDepthStencilValue
//		{
//			Depth,
//			Stencil
//		};
//		vkCmdClearDepthStencilImage(m_Handle, VkImage->Get(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &ClearDepthStencilValue, 1u, &SubresourceRange);
//
//		m_Barrier->TransitionResourceState(VkImage, EResourceState::TransferDst, VkImage->RequiredState, SubresourceRange);
//	}
}

void VulkanCommandBuffer::UpdateBuffer(const RHIBuffer* Buffer, const void* Data, const RHICopyRange& Range)
{
}

void VulkanCommandBuffer::UpdateImage(const RHIImage* Image, const void* Data, const RHICopyRange& Range)
{
}

//void VulkanCommandBuffer::SetPolygonMode(EPolygonMode Mode)
//{
//#if VK_EXT_extended_dynamic_state
//	if (m_Device->EnabledExtensions().ExtendedDynamicState)
//	{
//		assert(m_State == EState::Recording);
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
//	assert(Level() == ECommandBufferLevel::Primary && Commands.size() > 0u && m_State == EState::Recording);
//
//	std::vector<VkCommandBuffer> VkCommands;
//
//	std::transform(Commands.begin(), Commands.end(), VkCommands.begin(), [](std::shared_ptr<VulkanCommandBuffer> Command) {
//		assert(Command->Level() == ECommandBufferLevel::Secondary && Command->State() == EState::Pending || Command->State() == EState::Executable);
//		return Command->Get();
//		});
//
//	vkCmdExecuteCommands(m_Handle, static_cast<uint32_t>(VkCommands.size()), VkCommands.data());
//}

void VulkanCommandBuffer::WaitCommand(const RHICommandBuffer* CommandToWait)
{
	//auto VkCommand = static_cast<VulkanCommandBuffer*>(CommandToWait);
	//if (VkCommand && VkCommand != this)
	//{
	//	m_WaitSemaphores.emplace_back(VkCommand->Semaphore());
	//}
}

//void VulkanCommandBuffer::EnsurePipelineResourceStates(VulkanGraphicsPipelineState* State)
//{
//	assert(State && State->FrameBuffer);
//
//	for (auto& Image : State->FrameBuffer->Description().ColorAttachments)
//	{
//		auto VkImage = static_cast<VulkanImage*>(Image.get());
//		VkImage->RequiredState = EResourceState::RenderTarget;
//
//		m_Tracker.Images.insert(VkImage);
//
//		if (VkImage->PermanentState != EResourceState::Unknown)
//		{
//			m_Tracker.PermanentImages.insert(VkImage);
//		}
//	}
//
//	if (State->FrameBuffer->Description().DepthStencilAttachment)
//	{
//		auto VkImage = static_cast<VulkanImage*>(State->FrameBuffer->Description().DepthStencilAttachment.get());
//		VkImage->RequiredState = EResourceState::DepthWrite;
//
//		m_Tracker.Images.insert(VkImage);
//	}
//
//	for (auto& Resource : State->ResourcesNeedTransitionState)
//	{
//		if (Resource.Buffer)
//		{
//			Resource.Buffer->RequiredState = EResourceState::UniformBuffer;
//			m_Tracker.Buffers.insert(Resource.Buffer);
//		}
//
//		if (Resource.Image)
//		{
//			Resource.Image->RequiredState = EResourceState::ShaderResource;
//			m_Tracker.Images.insert(Resource.Image);
//		}
//	}
//
//	m_Tracker.EnsureCorrectStates(m_Barrier.get());
//}
//
//void VulkanCommandBuffer::VkResourceStateTracker::EnsureCorrectStates(VulkanPipelineBarrier* Barrier)
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
//			Barrier->TransitionResourceState(Image, Image->CurrentState, Image->RequiredState, SubresourceRange);
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
//			Barrier->TransitionResourceState(Buffer, Buffer->CurrentState, Buffer->RequiredState);
//		}
//	}
//	Buffers.clear();
//}
//
//NAMESPACE_END(RHI)
//
//#endif
