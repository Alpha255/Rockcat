#pragma once

#include "Runtime/Engine/RHI/RHIImage.h"

#define WHOLE_SIZE ~0u

struct RHIBufferCreateInfo
{
	ERHIBufferUsageFlags BufferUsageFlags = ERHIBufferUsageFlags::None;
	ERHIDeviceAccessFlags AccessFlags = static_cast<ERHIDeviceAccessFlags>(0u);
	ERHIResourceState RequiredState = ERHIResourceState::Common;
	size_t Size = 0u;
	const void* InitialData = nullptr;

	inline RHIBufferCreateInfo& SetUsages(ERHIBufferUsageFlags UsageFlags) { BufferUsageFlags = BufferUsageFlags | UsageFlags; return *this; }
	inline RHIBufferCreateInfo& SetAccessFlags(ERHIDeviceAccessFlags Flags) { AccessFlags = Flags | AccessFlags; return *this; }
	inline RHIBufferCreateInfo& SetSize(size_t DataSize) { Size = DataSize; return *this; }
	inline RHIBufferCreateInfo& SetInitialData(const void* Data) { InitialData = Data; return *this; }
};

class RHIBuffer : public RHIResource
{
public:
	using RHIResource::RHIResource;

	virtual void* Map(size_t Size = WHOLE_SIZE, size_t Offset = 0u) = 0;

	virtual void Unmap() = 0;

	virtual void FlushMappedRange(size_t Size = WHOLE_SIZE, size_t Offset = 0u) = 0;

	virtual void InvalidateMappedRange(size_t Size = WHOLE_SIZE, size_t Offset = 0u) = 0;

	virtual bool8_t Update(const void* Data, size_t Size, size_t SrcOffset = 0u, size_t DstOffset = 0u) = 0;

	void* GetMappedMemory() const
	{
		return m_MappedMemory;
	}
private:
	void* m_MappedMemory = nullptr;
};

struct RHIFrameBufferCreateInfo : public RHIHashedObject
{
	std::vector<const RHIImage*> ColorAttachments;
	const RHIImage* DepthStencilAttachment = nullptr;

	RHIFrameBufferCreateInfo& AddColorAttachment(const RHIImage* Attachment)
	{
		assert(Attachment);
		assert(std::find(ColorAttachments.begin(), ColorAttachments.end(), Attachment) == ColorAttachments.end());
		assert(ColorAttachments.size() < ERHILimitations::MaxRenderTargets);

		if (!Width || !Height || !Depth)
		{
			Width = Attachment->GetWidth();
			Height = Attachment->GetHeight();
			Depth = Attachment->GetDepth();
		}
		else
		{
			assert(Width == Attachment->GetWidth() && Height == Attachment->GetHeight() && Depth == Attachment->GetDepth());
		}

		ColorAttachments.emplace_back(Attachment);
		return *this;
	}

	RHIFrameBufferCreateInfo& SetDepthStencilAttachment(const RHIImage* Attachment)
	{
		assert(Attachment);

		if (!Width || !Height || !Depth)
		{
			Width = Attachment->GetWidth();
			Height = Attachment->GetHeight();
			Depth = Attachment->GetDepth();
		}
		else
		{
			assert(Width == Attachment->GetWidth() && Height == Attachment->GetHeight() && Depth == Attachment->GetDepth());
		}

		DepthStencilAttachment = Attachment;
		return *this;
	}

	size_t GetHash() const override final
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(
				Width,
				Height,
				Depth);

			HashCombine(HashValue, ColorAttachments.size());

			for (auto& Attachment : ColorAttachments)
			{
				assert(Attachment);
				HashCombine(HashValue, Attachment->GetFormat());
			}

			if (DepthStencilAttachment)
			{
				HashCombine(HashValue, DepthStencilAttachment->GetFormat());
			}
		}
		return HashValue;
	}

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t Depth = 0u;
};

class RHIFrameBuffer : public RHIResource
{
public:
	using RHIResource::RHIResource;

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline uint32_t GetDepth() const { return m_Depth; }
private:
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_Depth = 0u;
};