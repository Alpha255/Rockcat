#pragma once

#include "Colorful/IRenderer/IShader.h"
#include "Colorful/IRenderer/IBuffer.h"
#include "Colorful/IRenderer/IImage.h"

NAMESPACE_START(RHI)

enum class ECommandBufferLevel
{
	Primary,
	Secondary
};

class ICommandBuffer
{
public:
	ICommandBuffer(ECommandBufferLevel Level)
		: m_Level(Level)
	{
	}

	inline ECommandBufferLevel Level() const
	{
		return m_Level;
	}

	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Reset() = 0;

	virtual void BeginDebugMarker(const char8_t* Name, const Color& MarkerColor) = 0;
	virtual void EndDebugMarker() = 0;

	virtual void Draw(uint32_t VertexCount, uint32_t FirstVertex = 0u) = 0;
	virtual void DrawInstanced(uint32_t VertexCount, uint32_t InstanceCount, uint32_t FirstVertex = 0u, uint32_t FirstInstance = 0u) = 0;
	virtual void DrawIndirect(IBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) = 0;

	virtual void DrawIndexed(uint32_t IndexCount, uint32_t FirstIndex, int32_t VertexOffset) = 0;
	virtual void DrawIndexedInstanced(uint32_t IndexCount, uint32_t InstanceCount, uint32_t FirstIndex, int32_t VertexOffset, uint32_t FirstInstance = 0u) = 0;
	virtual void DrawIndexedIndirect(IBuffer* IndirectBuffer, size_t Offset, uint32_t DrawCount, uint32_t Stride) = 0;

	virtual void Dispatch(uint32_t GroupX, uint32_t GroupY, uint32_t GroupZ) = 0;
	virtual void DispatchIndirect(IBuffer* IndirectBuffer, size_t Offset) = 0;
	
	virtual void PushConstants(EShaderStage DstStage, IBuffer* ConstantsBuffer, const void* SrcData, size_t Size, size_t Offset = 0u) = 0;

	virtual void SetGraphicsPipeline(IPipeline* DstPipeline) = 0;

	virtual void ClearColorImage(IImage* DstImage, const Color& ClearColor) = 0;

	virtual void ClearDepthStencilImage(IImage* DstImage, bool8_t ClearDepth = true, bool8_t ClearStencil = true, float32_t Depth = 1.0f, uint8_t Stencil = 0xF) = 0;

	virtual void CopyBufferToImage(IImage* DstImage, const void* SrcBuffer, uint32_t BufferSize, const ImageSubresourceRange& SubresourceRange) = 0;

	virtual void CopyBuffer(IBuffer* DstBuffer, const void* SrcData, size_t DataSize, size_t SrcOffset = 0u, size_t DstOffset = 0u) = 0;

	virtual void CopyImage(IImage* SrcImage, const ImageSlice& SrcSlice, IImage* DstImage, const ImageSlice& DstSlice) = 0;

	virtual void SetViewport(const Viewport& DstViewport) = 0;

	virtual void SetScissorRect(const ScissorRect& DstScissorRect) = 0;

	virtual void WaitCommand(ICommandBuffer* CommandToWait) = 0;
protected:
private:
	ECommandBufferLevel m_Level = ECommandBufferLevel::Primary;
};

class ICommandBufferPool
{
};

class ICommandList
{
};

NAMESPACE_END(RHI)
