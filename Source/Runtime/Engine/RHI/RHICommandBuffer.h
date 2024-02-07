#pragma once

#include "Core/Math/Color.h"
#include "Engine/RHI/RHIShader.h"
#include "Engine/RHI/RHIPipeline.h"

enum class ERHICommandBufferLevel
{
	Primary,
	Secondary
};

class RHICommandBuffer
{
public:
	enum class EState
	{
		Initial,
		Recording,
		Executable
	};

	RHICommandBuffer(ERHICommandBufferLevel Level)
		: m_Level(Level)
	{
	}

	inline ERHICommandBufferLevel GetLevel() const { return m_Level; }

	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Reset() = 0;

	virtual void BeginDebugMarker(const char* Name, const Math::Color& Color) = 0;
	virtual void EndDebugMarker() = 0;

	virtual void SetVertexBuffer(const RHIBuffer* Buffer, uint32_t StartSlot, size_t Offset) = 0;
	virtual void SetIndexBuffer(const RHIBuffer* Buffer, size_t Offset, ERHIIndexFormat Format) = 0;
	virtual void SetPrimitiveTopology(ERHIPrimitiveTopology Topology) = 0;

	virtual void Draw(uint32_t VertexCount, uint32_t FirstVertex = 0u) = 0;
	virtual void DrawInstanced(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex = 0u, uint32_t FirstInstance = 0u) = 0;
	virtual void DrawIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) = 0;

	virtual void DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, int32_t VertexOffset) = 0;
	virtual void DrawIndexedInstanced(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, int32_t VertexOffset, uint32_t FirstInstance = 0u) = 0;
	virtual void DrawIndexedIndirect(const RHIBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) = 0;

	virtual void Dispatch(uint32_t GroupX, uint32_t GroupY, uint32_t GroupZ) = 0;
	virtual void DispatchIndirect(const RHIBuffer* IndirectBuffer, size_t Offset) = 0;
	
	virtual void PushConstants(ERHIShaderStage DstStage, const RHIBuffer* ConstantsBuffer, const void* SrcData, size_t Size, size_t Offset = 0u) = 0;

	virtual void SetGraphicsPipeline(const RHIGraphicsPipeline* DstPipeline) = 0;

	virtual void ClearColorImage(const RHIImage* DstImage, const Math::Color& ClearColor) = 0;

	virtual void ClearDepthStencilImage(const RHIImage* DstImage, bool ClearDepth = true, bool ClearStencil = true, float Depth = 1.0f, uint8_t Stencil = 0xF) = 0;

	virtual void CopyBufferToImage(const RHIImage* DstImage, const void* SrcBuffer, uint32_t BufferSize, const RHIImageSubresourceRange& SubresourceRange) = 0;

	virtual void CopyBuffer(const RHIBuffer* DstBuffer, const void* SrcData, size_t DataSize, size_t SrcOffset = 0u, size_t DstOffset = 0u) = 0;

	//virtual void CopyImage(RHIImage* SrcImage, const ImageSlice& SrcSlice, IImage* DstImage, const ImageSlice& DstSlice) = 0;

	virtual void SetViewport(const RHIViewport& DstViewport) = 0;

	virtual void SetScissorRect(const RHIScissorRect& DstScissorRect) = 0;

	virtual void WaitCommand(const RHICommandBuffer* CommandToWait) = 0;
protected:
	inline void SetState(EState State) { m_State = State; }
	EState m_State = EState::Initial;
private:
	ERHICommandBufferLevel m_Level = ERHICommandBufferLevel::Primary;
};

class RHICommandBufferPool
{
};

class RHICommandList
{
};
