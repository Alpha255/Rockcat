#pragma once

#include "Colorful/IRenderer/IDevice.h"
#include "Colorful/D3D/D3D12/D3D12Types.h"

NAMESPACE_START(RHI)

DECLARE_D3D_HWOBJECT(D3D12CommandList1, ID3D12GraphicsCommandList1)
DECLARE_D3D_HWOBJECT(D3D12CommandList2, ID3D12GraphicsCommandList2)
DECLARE_D3D_HWOBJECT(D3D12CommandList3, ID3D12GraphicsCommandList3)
DECLARE_D3D_HWOBJECT(D3D12CommandList4, ID3D12GraphicsCommandList4)
DECLARE_D3D_HWOBJECT(D3D12CommandList5, ID3D12GraphicsCommandList5)
DECLARE_D3D_HWOBJECT(D3D12CommandList6, ID3D12GraphicsCommandList6)

class D3D12CommandAllocator : public D3DHWObject<void, ID3D12CommandAllocator>
{
};

class D3D12CommandList : public D3DHWObject<ICommandBuffer, ID3D12GraphicsCommandList>
{
public:
	enum class EState
	{
		Initial,
		Recording,
		Closed
	};

	D3D12CommandList(class D3D12Device* Device, EQueueType QueueType, ECommandBufferLevel Level);

	~D3D12CommandList();

	void Begin() override final;
	void End() override final;
	void Reset();

	void BeginDebugMarker(const char8_t* Name, const Color& MarkerColor) override final;
	void EndDebugMarker() override final;

	void Draw(uint32_t VertexCount, uint32_t FirstVertex) override final;
	void DrawInstanced(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance);
	void DrawIndirect(IBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride);

	void DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, int32_t VertexOffset) override final;
	void DrawIndexedInstanced(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, int32_t VertexOffset, uint32_t FirstInstance) override final;
	void DrawIndexedIndirect(IBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) override final;

	void Dispatch(uint32_t GroupX, uint32_t GroupY, uint32_t GroupZ) override final;
	void DispatchIndirect(IBuffer* IndirectBuffer, size_t Offset) override final;

	void PushConstants(EShaderStage Stage, IBuffer* ConstantsBuffer, const void* Data, size_t Size, size_t Offset) override final;

	void SetGraphicsPipeline(IPipeline* DstPipeline) override final;

	void ClearColorImage(IImage* DstImage, const Color& ClearColor) override final;

	void ClearDepthStencilImage(IImage* DstImage, bool8_t ClearDepth, bool8_t ClearStencil, float32_t Depth, uint8_t Stencil) override final;

	void CopyBufferToImage(IImage* DstImage, const void* SrcBuffer, uint32_t BufferSize, const ImageSubresourceRange& SubresourceRange) override final;

	void CopyBuffer(IBuffer* DstBuffer, const void* Data, size_t DataSize, size_t SrcOffset, size_t DstOffset) override final;

	void CopyImage(IImage* SrcImage, const ImageSlice& SrcSlice, IImage* DstImage, const ImageSlice& DstSlice) override final;

	void SetViewport(const Viewport& DstViewport) override final;

	void SetScissorRect(const ScissorRect& DstScissorRect) override final;

	void WaitCommand(ICommandBuffer* CommandToWait) override final;
protected:
	inline void SetState(EState State)
	{
		m_State = State;
	}
private:
	D3D12CommandAllocator m_Allocator;
	EState m_State = EState::Initial;
	class D3D12GraphicsPipeline* m_CurGfxPipeline = nullptr;
};

NAMESPACE_END(RHI)
