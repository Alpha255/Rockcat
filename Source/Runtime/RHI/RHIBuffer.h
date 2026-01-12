#pragma once

#include "RHI/RHITexture.h"
#include "Asset/Asset.h"

#define RHI_WHOLE_SIZE ~0u

struct RHIBufferDesc
{
	ERHIBufferUsageFlags BufferUsageFlags = ERHIBufferUsageFlags::None;
	ERHIDeviceAccessFlags AccessFlags = ERHIDeviceAccessFlags::None;
	ERHIResourceState PermanentStates = ERHIResourceState::Unknown;

	size_t Size = 0ull;
	const void* InitialData = nullptr;

	FName Name;

	inline RHIBufferDesc& SetUsages(ERHIBufferUsageFlags UsageFlags) { BufferUsageFlags = BufferUsageFlags | UsageFlags; return *this; }
	inline RHIBufferDesc& SetAccessFlags(ERHIDeviceAccessFlags Flags) { AccessFlags = Flags | AccessFlags; return *this; }
	inline RHIBufferDesc& SetPermanentStates(ERHIResourceState States) { PermanentStates = States; return *this; }
	inline RHIBufferDesc& SetSize(size_t InSize) { Size = InSize; return *this; }
	inline RHIBufferDesc& SetInitialData(const void* Data) { InitialData = Data; return *this; }
	inline RHIBufferDesc& SetName(FName&& InName) { Name = std::move(InName); return *this; }
};

enum class ERHIMapMode
{
	ReadOnly,
	WriteOnly
};

struct RHIMappedMemory
{
	void* Memory = nullptr;
	size_t Size = 0u;
	size_t Offset = 0u;
	ERHIMapMode Mode = ERHIMapMode::ReadOnly;
};

class RHIBuffer : public RHIResource
{
public:
	RHIBuffer(const RHIBufferDesc& Desc)
		: RHIResource(Desc.Name)
		, m_Size(Desc.Size)
	{
	}

	virtual void* Map(ERHIMapMode Mode, size_t Size = RHI_WHOLE_SIZE, size_t Offset = 0u) = 0;
	virtual void Unmap() = 0;
	virtual void FlushMappedRange(size_t Size = RHI_WHOLE_SIZE, size_t Offset = 0u) = 0;
	virtual void InvalidateMappedRange(size_t Size = RHI_WHOLE_SIZE, size_t Offset = 0u) = 0;
	virtual RHIBufferPtr Suballocate(const RHIBufferDesc&) { return nullptr; }
	
	inline void* GetMapped() const { return m_MappedMemory.Memory; }
	inline size_t GetSize() const { return m_Size; }
protected:
	size_t m_Size = 0u;
	RHIMappedMemory m_MappedMemory;
};

struct RHIFrameBufferDesc
{
	struct RHIAttachment
	{
		const RHITexture* Texture = nullptr;
		RHISubresource Subresource = RHI::AllSubresource;

		uint32_t GetWidth() const { assert(Texture); return Texture->GetWidth() / (1 << Subresource.BaseMipLevel); }
		uint32_t GetHeight() const { assert(Texture); return Texture->GetHeight() / (1 << Subresource.BaseMipLevel); }
		uint32_t GetArrayLayers() const { return Subresource.NumLayers; }
	};

	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t ArrayLayers = 0u;

	RHIAttachment DepthStencilAttachment;
	std::vector<RHIAttachment> ColorAttachments;

	FName Name;

	inline uint32_t GetNumColorAttachments() const { return NumColorAttachments; }

	inline bool HasDepthStencil() const { return DepthStencilAttachment.Texture != nullptr && (RHI::IsDepth(DepthStencilAttachment.Texture->GetFormat()) || RHI::IsDepthStencil(DepthStencilAttachment.Texture->GetFormat())); }

	inline RHIFrameBufferDesc& AddColorAttachment(const RHITexture* Texture, RHISubresource Subresource = RHI::AllSubresource)
	{
		assert(Texture && ColorAttachments.size() < (ERHILimitations::MaxRenderTargets - 1u) && RHI::IsColor(Texture->GetFormat()));

		auto& Attachment = ColorAttachments.emplace_back(RHIAttachment{
			Texture,
			Subresource
		});

		SetWidth(Attachment.GetWidth());
		SetHeight(Attachment.GetHeight());
		SetArrayLayers(Attachment.GetArrayLayers());
		++NumColorAttachments;

		return *this;
	}

	inline RHIFrameBufferDesc& SetColorAttachment(uint32_t Index, const RHITexture* Texture, RHISubresource Subresource = RHI::AllSubresource)
	{
		assert(Texture && Index < ColorAttachments.size() && Index < ERHILimitations::MaxRenderTargets && RHI::IsColor(Texture->GetFormat()));

		auto& Attachment = ColorAttachments[Index];
		Attachment.Texture = Texture;
		Attachment.Subresource = Subresource;

		SetWidth(Attachment.GetWidth());
		SetHeight(Attachment.GetHeight());
		SetArrayLayers(Attachment.GetArrayLayers());

		return *this;
	}

	inline RHIFrameBufferDesc& SetDepthStencilAttachment(const RHITexture* Texture, RHISubresource Subresource = RHI::AllSubresource)
	{
		assert(Texture && (RHI::IsDepth(Texture->GetFormat()) || RHI::IsDepthStencil(Texture->GetFormat())));

		DepthStencilAttachment.Texture = Texture;
		DepthStencilAttachment.Subresource = Subresource;

		SetWidth(DepthStencilAttachment.GetWidth());
		SetHeight(DepthStencilAttachment.GetHeight());
		SetArrayLayers(DepthStencilAttachment.GetArrayLayers());

		return *this;
	}

	inline RHIFrameBufferDesc& AddAttachment(const RHITexture* Texture, RHISubresource Subresource = RHI::AllSubresource)
	{
		assert(Texture);

		if (RHI::IsColor(Texture->GetFormat()))
		{
			AddColorAttachment(Texture, Subresource);
		}
		else
		{
			assert(!HasDepthStencil());
			SetDepthStencilAttachment(Texture, Subresource);
		}

		return *this;
	}

	inline RHIFrameBufferDesc& SetName(FName&& InName) { Name = std::move(InName); return *this; }
private:
	uint32_t NumColorAttachments = 0u;

	void SetWidth(uint32_t InWidth)
	{
		assert(Width == 0 || InWidth == Width);
		Width = InWidth;
	}
	void SetHeight(uint32_t InHeight)
	{
		assert(Height == 0 || InHeight == Height);
		Height = InHeight;
	}
	void SetArrayLayers(uint32_t InArrayLayers)
	{
		assert(ArrayLayers == 0 || InArrayLayers == ArrayLayers);
		ArrayLayers = InArrayLayers;
	}
};

class RHIFrameBuffer : public RHIResource
{
public:
	RHIFrameBuffer(const RHIFrameBufferDesc& Desc)
		: RHIResource(Desc.Name)
	{
	}

	inline uint32_t GetWidth() const { return m_Width; }
	inline uint32_t GetHeight() const { return m_Height; }
	inline uint32_t GetArrayLayers() const { return m_ArrayLayers; }
private:
	uint32_t m_Width = 0u;
	uint32_t m_Height = 0u;
	uint32_t m_ArrayLayers = 0u;
};
