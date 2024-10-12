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
	
	virtual void PushConstants(ERHIShaderStage Stage, const RHIBuffer* Buffer, const void* SrcData, size_t Size, size_t Offset = 0u) = 0;

	virtual void SetGraphicsPipeline(const RHIGraphicsPipeline* Pipeline) = 0;

	virtual void ClearColorImage(const RHIImage* Image, const Math::Color& ClearColor) = 0;

	virtual void ClearDepthStencilImage(const RHIImage* Image, bool ClearDepth = true, bool ClearStencil = true, float Depth = 1.0f, uint8_t Stencil = 0xF) = 0;

	virtual void UpdateBuffer(const RHIBuffer* Buffer, const void* Data, size_t Size, size_t Offset = 0u) = 0;
	virtual void UpdateImage(const RHIImage* Image, const void* Data, size_t Size, size_t Offset = 0u) = 0;

	virtual void SetViewport(const RHIViewport& Viewport) = 0;
	virtual void SetViewports(const RHIViewport* Viewports, uint32_t NumViewports) = 0u;

	virtual void SetScissorRect(const RHIScissorRect& ScissorRect) = 0;
	virtual void SetScissorRects(const RHIScissorRect* ScissorRects, uint32_t NumScissorRects) = 0;

	virtual void WaitCommand(const RHICommandBuffer* CommandToWait) = 0;

	EState GetState() const { return m_State; }
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
