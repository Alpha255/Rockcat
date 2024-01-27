#pragma once

#include "Runtime/Engine/RHI/RHIImage.h"

#define WHOLE_SIZE ~0u

struct RHIBufferCreateInfo
{
	ERHIBufferUsageFlags BufferUsageFlags = ERHIBufferUsageFlags::None;
	ERHIDeviceAccessFlags AccessFlags = static_cast<ERHIDeviceAccessFlags>(0u);
	size_t Size = 0u;
	const void* InitialData = nullptr;
	std::string Name;

	inline RHIBufferCreateInfo& SetUsages(ERHIBufferUsageFlags UsageFlags) { BufferUsageFlags = BufferUsageFlags | UsageFlags; return *this; }
	inline RHIBufferCreateInfo& SetAccessFlags(ERHIDeviceAccessFlags Flags) { AccessFlags = Flags | AccessFlags; return *this; }
	inline RHIBufferCreateInfo& SetSize(size_t DataSize) { Size = DataSize; return *this; }
	inline RHIBufferCreateInfo& SetInitialData(const void* Data) { InitialData = Data; return *this; }
	inline RHIBufferCreateInfo& SetName(const char* InName) { Name = InName; return *this; }
};

class RHIBuffer
{
public:
	virtual void* Map(size_t Size = WHOLE_SIZE, size_t Offset = 0u) = 0;
	virtual void Unmap() = 0;
	virtual void FlushMappedRange(size_t Size = WHOLE_SIZE, size_t Offset = 0u) = 0;
	virtual void InvalidateMappedRange(size_t Size = WHOLE_SIZE, size_t Offset = 0u) = 0;
	virtual bool8_t Update(const void* Data, size_t Size, size_t SrcOffset = 0u, size_t DstOffset = 0u) = 0;
	void* GetMappedMemory() const { return m_MappedMemory; }
protected:
	void* m_MappedMemory = nullptr;
};

struct RHIFrameBufferCreateInfo
{
	struct RHIAttachment
	{
		ERHIFormat Format = ERHIFormat::Unknown;
		RHIImagePtr Image;
	};

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t ArrayLayers = 0u;

	RHIAttachment DepthStencilAttachment;
	std::vector<RHIAttachment> ColorAttachments;

	RHIFrameBufferCreateInfo& SetWidth(uint32_t InWidth) { Width = InWidth; }
	RHIFrameBufferCreateInfo& SetHeight(uint32_t InHeight) { Height = InHeight; }
	RHIFrameBufferCreateInfo& SetArrayLayers(uint32_t InLayers) { ArrayLayers = InLayers; }

	RHIFrameBufferCreateInfo& AddColorFormat(ERHIFormat Format)
	{
		assert(ColorAttachments.size() < ERHILimitations::MaxRenderTargets);
		ColorAttachments.emplace_back(RHIAttachment{Format, nullptr});
		return *this;
	}

	RHIFrameBufferCreateInfo& SetColorAttachment(uint32_t Index, RHIImagePtr Color)
	{
		assert(Index < ERHILimitations::MaxRenderTargets && ColorAttachments[Index].Format == Color->GetFormat());
		ColorAttachments[Index].Image = Color;
		return *this;
	}

	RHIFrameBufferCreateInfo& SetDepthStencilFormat(ERHIFormat Format)
	{
		DepthStencilAttachment.Format = Format;
		return *this;
	}

	RHIFrameBufferCreateInfo& SetDepthStencilAttachment(RHIImagePtr DepthStencil)
	{
		assert(DepthStencilAttachment.Format == DepthStencil->GetFormat());
		DepthStencilAttachment.Image = DepthStencil;
		return *this;
	}
};

class RHIFrameBuffer
{
public:
	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline uint32_t GetArrayLayers() const { return m_ArrayLayers; }
private:
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_ArrayLayers = 0u;
};
