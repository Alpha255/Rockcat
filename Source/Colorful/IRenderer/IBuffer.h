#pragma once

#include "Colorful/IRenderer/IImage.h"

NAMESPACE_START(RHI)

#define WHOLE_SIZE ~0ull

struct BufferDesc
{
	EBufferUsageFlags UsageFlags = EBufferUsageFlags::None;
	EDeviceAccessFlags AccessFlags = static_cast<EDeviceAccessFlags>(0u);
	EResourceState RequiredState = EResourceState::Common;
	size_t Size = 0u;
	const void* InitialData = nullptr;

	inline BufferDesc& SetUsages(EBufferUsageFlags Flags) { UsageFlags = Flags | UsageFlags; return *this; }
	inline BufferDesc& SetAccessFlags(EDeviceAccessFlags Flags) { AccessFlags = Flags | AccessFlags; return *this; }
	inline BufferDesc& SetSize(size_t DataSize) { Size = DataSize; return *this; }
	inline BufferDesc& SetInitialData(const void* Data) { InitialData = Data; return *this; }
};

class IBuffer : public IHWResource
{
public:
	virtual void* Map(size_t Size = WHOLE_SIZE, size_t Offset = 0u) = 0;

	virtual void Unmap() = 0;

	virtual void FlushMappedRange(size_t Size = WHOLE_SIZE, size_t Offset = 0u) = 0;

	virtual void InvalidateMappedRange(size_t Size = WHOLE_SIZE, size_t Offset = 0u) = 0;

	virtual bool8_t Update(const void* Data, size_t Size, size_t SrcOffset = 0u, size_t DstOffset = 0u) = 0;

	void* MappedMemory() const
	{
		return m_MappedMemory;
	}
protected:
	void* m_MappedMemory = nullptr;
};

class ScopedBufferMapper
{
public:
	ScopedBufferMapper(IBuffer* Buffer, size_t Size, size_t Offset)
		: m_Buffer(Buffer)
	{
		assert(m_Buffer);
		m_Buffer->Map(Size, Offset);
	}

	~ScopedBufferMapper()
	{
		m_Buffer->Unmap();
	}
protected:
private:
	IBuffer* m_Buffer = nullptr;
};

struct FrameBufferDesc : public IHashedResourceDesc
{
	std::vector<IImagePtr> ColorAttachments;
	IImagePtr DepthStencilAttachment;

	FrameBufferDesc& AddColorAttachment(const IImagePtr& Attachment)
	{
		assert(Attachment);
		assert(std::find(ColorAttachments.begin(), ColorAttachments.end(), Attachment) == ColorAttachments.end());
		assert(ColorAttachments.size() < ELimitations::MaxRenderTargets);

		if (!Width || !Height || !Depth)
		{
			Width = Attachment->Width();
			Height = Attachment->Height();
			Depth = Attachment->Depth();
		}
		else
		{
			assert(Width == Attachment->Width() && Height == Attachment->Height() && Depth == Attachment->Depth());
		}

		ColorAttachments.emplace_back(Attachment);
		return *this;
	}

	FrameBufferDesc& SetDepthStencilAttachment(const IImagePtr& Attachment)
	{
		assert(Attachment);

		if (!Width || !Height || !Depth)
		{
			Width = Attachment->Width();
			Height = Attachment->Height();
			Depth = Attachment->Depth();
		}
		else
		{
			assert(Width == Attachment->Width() && Height == Attachment->Height() && Depth == Attachment->Depth());
		}

		DepthStencilAttachment = Attachment;
		return *this;
	}

	size_t Hash() const override final
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
				HashCombine(HashValue, Attachment->Format());
			}

			if (DepthStencilAttachment)
			{
				HashCombine(HashValue, DepthStencilAttachment->Format());
			}
		}

		return HashValue;
	}

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t Depth = 0u;
};

class IFrameBuffer : public IHWResource
{
public:
	IFrameBuffer(uint32_t Width, uint32_t Height, uint32_t Depth, const FrameBufferDesc& Desc)
		: m_Width(Width)
		, m_Height(Height)
		, m_Depth(Depth)
		, m_Desc(Desc)
	{
	}

	const uint32_t Width() const
	{
		return m_Width;
	}

	const uint32_t Height() const
	{
		return m_Height;
	}

	const uint32_t Depth() const
	{
		return m_Depth;
	}

	const FrameBufferDesc& Description() const
	{
		return m_Desc;
	}
protected:
private:
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_Depth = 0u;
	FrameBufferDesc m_Desc;
};

NAMESPACE_END(RHI)
