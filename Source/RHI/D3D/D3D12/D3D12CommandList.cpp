#include "RHI/D3D/D3D12/D3D12CommandList.h"
#include "RHI/D3D/D3D12/D3D12Device.h"
#include "RHI/D3D/D3D12/D3D12Pipeline.h"
#include "RHI/D3D/D3D12/D3D12Buffer.h"
#include "Runtime/Engine/Services/SpdLogService.h"

#include <pix.h>

D3D12CommandList::D3D12CommandList(const D3D12Device& Device, ERHIDeviceQueue QueueType, ERHICommandBufferLevel Level)
	: RHICommandBuffer(Level)
{
	D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	if (Level == ERHICommandBufferLevel::Secondary)
	{
		Type = D3D12_COMMAND_LIST_TYPE_BUNDLE;
	}
	else
	{
		switch (QueueType)
		{
		case ERHIDeviceQueue::Graphics:
			Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			break;
		case ERHIDeviceQueue::Transfer:
			Type = D3D12_COMMAND_LIST_TYPE_COPY;
			break;
		case ERHIDeviceQueue::Compute:
			Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
			break;
		}
	}

	VERIFY_D3D(Device->CreateCommandAllocator(Type, IID_PPV_ARGS(m_Allocator.Reference())));
	VERIFY_D3D(Device->CreateCommandList(0u, Type, m_Allocator.GetNative(), nullptr, IID_PPV_ARGS(Reference())));

	SetState(EState::Initial);
}

void D3D12CommandList::Begin()
{
	assert(m_State == EState::Initial || m_State == EState::Closed);

	Reset(); /// #TODO

	SetState(EState::Recording);
}

void D3D12CommandList::End()
{
	assert(m_State == EState::Recording);

	VERIFY_D3D(GetNative()->Close());

	SetState(EState::Closed);
}

void D3D12CommandList::Reset()
{
	assert(m_State == EState::Initial || m_State == EState::Closed);

	/// Resets a command list back to its initial state as if a new command list was just created.
	VERIFY_D3D(GetNative()->Reset(m_Allocator.GetNative(), nullptr));

	SetState(EState::Initial);
}

void D3D12CommandList::BeginDebugMarker(const char8_t* Name, const Math::Color& Color)
{
	uint32_t RGBA = Color.RGBA8();
	PIXBeginEvent(GetNative(), PIX_COLOR(uint8_t(RGBA && 0x000F), uint8_t(RGBA && 0x00F0 >> 8), uint8_t(RGBA & 0x0F00 >> 16)), Name);
}

void D3D12CommandList::EndDebugMarker()
{
	PIXEndEvent(GetNative());
}

void D3D12CommandList::Draw(uint32_t VertexCount, uint32_t FirstVertex)
{
	assert(m_State == EState::Recording);
	GetNative()->DrawInstanced(VertexCount, 1u, FirstVertex, 0u);
}

void D3D12CommandList::DrawInstanced(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance)
{
	assert(m_State == EState::Recording);
	GetNative()->DrawInstanced(VertexCount, InstanceCount, FirstVertex, FirstInstance);
}

void D3D12CommandList::DrawIndirect(RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride)
{
	assert(m_State == EState::Recording);
	assert(false); /// #TODO

	(void)Stride;
	GetNative()->ExecuteIndirect(nullptr, DrawCount, Cast<D3D12Buffer>(IndirectBuffer)->GetNative(), Offset, nullptr, 0u);
}

void D3D12CommandList::DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, int32_t VertexOffset)
{
	assert(m_State == EState::Recording);
	GetNative()->DrawIndexedInstanced(IndexCount, 1, FirstIndex, VertexOffset, 0u);
}

void D3D12CommandList::DrawIndexedInstanced(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, int32_t VertexOffset, uint32_t FirstInstance)
{
	assert(m_State == EState::Recording);
	GetNative()->DrawIndexedInstanced(IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
}

void D3D12CommandList::DrawIndexedIndirect(RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride)
{
	assert(m_State == EState::Recording);
	assert(false); /// #TODO

	(void)Stride;
	GetNative()->ExecuteIndirect(nullptr, DrawCount, Cast<D3D12Buffer>(IndirectBuffer)->GetNative(), Offset, nullptr, 0u);
}

void D3D12CommandList::Dispatch(uint32_t GroupX, uint32_t GroupY, uint32_t GroupZ)
{
	assert(m_State == EState::Recording);
	GetNative()->Dispatch(GroupX, GroupY, GroupZ);
}

void D3D12CommandList::DispatchIndirect(RHIBuffer* IndirectBuffer, size_t Offset)
{
	assert(m_State == EState::Recording);
	assert(false); /// #TODO

	GetNative()->ExecuteIndirect(nullptr, 1u, Cast<D3D12Buffer>(IndirectBuffer)->GetNative(), Offset, nullptr, 0u);
}

void D3D12CommandList::PushConstants(ERHIShaderStage Stage, RHIBuffer* ConstantsBuffer, const void* Data, size_t Size, size_t Offset)
{
}

//void D3D12CommandList::SetGraphicsPipeline(IPipeline* DstPipeline)
//{
//	assert(m_State == EState::Recording && DstPipeline);
//
//	auto D3D12Pipeline = static_cast<D3D12GraphicsPipeline*>(DstPipeline);
//	assert(D3D12Pipeline);
//}

void D3D12CommandList::ClearColorImage(RHIImage* DstImage, const Math::Color& ClearColor)
{
}

void D3D12CommandList::ClearDepthStencilImage(RHIImage* DstImage, bool8_t ClearDepth, bool8_t ClearStencil, float32_t Depth, uint8_t Stencil)
{
}

void D3D12CommandList::CopyBufferToImage(RHIImage* DstImage, const void* SrcBuffer, uint32_t BufferSize, const RHIImageSubresourceRange& SubresourceRange)
{
}

void D3D12CommandList::CopyBuffer(RHIBuffer* DstBuffer, const void* Data, size_t DataSize, size_t SrcOffset, size_t DstOffset)
{
}

//void D3D12CommandList::CopyImage(IImage* SrcImage, const ImageSlice& SrcSlice, IImage* DstImage, const ImageSlice& DstSlice)
//{
//}

void D3D12CommandList::SetViewport(const RHIViewport& DstViewport)
{
}

void D3D12CommandList::SetScissorRect(const RHIScissorRect& DstScissorRect)
{
}

void D3D12CommandList::WaitCommand(RHICommandBuffer* CommandToWait)
{
}

D3D12CommandList::~D3D12CommandList()
{
}
