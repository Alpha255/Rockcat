#pragma once

#include "Engine/RHI/RHICommandBuffer.h"
#include "Engine/RHI/RHIDevice.h"
#include "RHI/D3D/D3D12/D3D12Types.h"

DECLARE_D3D_HWOBJECT(D3D12CommandList1, ID3D12GraphicsCommandList1)
DECLARE_D3D_HWOBJECT(D3D12CommandList2, ID3D12GraphicsCommandList2)
DECLARE_D3D_HWOBJECT(D3D12CommandList3, ID3D12GraphicsCommandList3)
DECLARE_D3D_HWOBJECT(D3D12CommandList4, ID3D12GraphicsCommandList4)
DECLARE_D3D_HWOBJECT(D3D12CommandList5, ID3D12GraphicsCommandList5)
DECLARE_D3D_HWOBJECT(D3D12CommandList6, ID3D12GraphicsCommandList6)

class D3D12CommandAllocator : public D3DHwResource<ID3D12CommandAllocator>
{
};

class D3D12CommandList : public D3DHwResource<ID3D12GraphicsCommandList>, public RHICommandBuffer
{
public:
	D3D12CommandList(const class D3D12Device& Device, ERHIDeviceQueue QueueType, ERHICommandBufferLevel Level);

	~D3D12CommandList();

	void Begin() override final;
	void End() override final;
	void Reset();

	void BeginDebugMarker(const char* Name, const Math::Color& Color) override final;
	void EndDebugMarker() override final;

	void SetVertexBuffer(const RHIBuffer* Buffer, uint32_t StartSlot, size_t Offset) override final;
	void SetVertexStream(uint32_t StartSlot, const RHIVertexStream& VertexStream) override final;
	void SetIndexBuffer(const RHIBuffer* Buffer, size_t Offset, ERHIIndexFormat Format) override final;
	void SetPrimitiveTopology(ERHIPrimitiveTopology Topology) override final;

	void Draw(uint32_t VertexCount, uint32_t FirstVertex) override final;
	void DrawInstanced(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex, uint32_t FirstInstance);
	void DrawIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride);

	void DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, int32_t VertexOffset) override final;
	void DrawIndexedInstanced(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, int32_t VertexOffset, uint32_t FirstInstance) override final;
	void DrawIndexedIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) override final;

	void Dispatch(uint32_t GroupX, uint32_t GroupY, uint32_t GroupZ) override final;
	void DispatchIndirect(const RHIBuffer* IndirectBuffer, size_t Offset) override final;

	void PushConstants(ERHIShaderStage Stage, const RHIBuffer* ConstantsBuffer, const void* Data, size_t Size, size_t Offset) override final;

	void SetGraphicsPipeline(const RHIGraphicsPipeline* GraphicsPipeline) override final;

	void ClearColorTexture(const RHITexture* Texture, const Math::Color& ClearColor) override final;
	void ClearDepthStencilTexture(const RHITexture* Texture, bool ClearDepth, bool ClearStencil, float Depth, uint8_t Stencil) override final;

	void WriteBuffer(const RHIBuffer* Buffer, const void* Data, size_t Size, size_t Offset) override final;
	void WriteTexture(const RHITexture* Texture, const void* Data, size_t Size, size_t Offset) override final;

	void SetViewport(const RHIViewport& Viewport) override final;
	void SetViewports(const RHIViewport* Viewports, uint32_t NumViewports) override final;

	void SetScissorRect(const RHIScissorRect& ScissorRect) override final;
	void SetScissorRects(const RHIScissorRect* ScissorRects, uint32_t NumScissorRects) override final;

	void WaitCommand(const RHICommandBuffer* CommandToWait) override final;
private:
	D3D12CommandAllocator m_Allocator;
	class D3D12GraphicsPipeline* m_CurGfxPipeline = nullptr;
};
