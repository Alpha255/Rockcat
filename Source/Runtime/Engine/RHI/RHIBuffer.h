#pragma once

#include "Engine/RHI/RHITexture.h"
#include "Engine/Asset/Asset.h"

#define RHI_WHOLE_SIZE ~0u

struct RHIBufferCreateInfo
{
	ERHIBufferUsageFlags BufferUsageFlags = ERHIBufferUsageFlags::None;
	ERHIDeviceAccessFlags AccessFlags = ERHIDeviceAccessFlags::None;
	ERHIResourceState PermanentStates = ERHIResourceState::Unknown;

	size_t Size = 0ull;
	const void* InitialData = nullptr;

	std::string Name;

	inline RHIBufferCreateInfo& SetUsages(ERHIBufferUsageFlags UsageFlags) { BufferUsageFlags = BufferUsageFlags | UsageFlags; return *this; }
	inline RHIBufferCreateInfo& SetAccessFlags(ERHIDeviceAccessFlags Flags) { AccessFlags = Flags | AccessFlags; return *this; }
	inline RHIBufferCreateInfo& SetPermanentStates(ERHIResourceState States) { PermanentStates = States; return *this; }
	inline RHIBufferCreateInfo& SetSize(size_t InSize) { Size = InSize; return *this; }
	inline RHIBufferCreateInfo& SetInitialData(const void* Data) { InitialData = Data; return *this; }

	template<class T>
	inline RHIBufferCreateInfo& SetName(T&& InName) { Name = std::move(std::string(std::forward<T>(InName))); return *this; }
};

class RHIBuffer : public RHIResource
{
public:
	RHIBuffer(const RHIBufferCreateInfo& RHICreateInfo)
		: RHIResource(RHICreateInfo.Name.c_str())
	{
	}

	virtual void* Map(size_t Size = RHI_WHOLE_SIZE, size_t Offset = 0u) = 0;
	virtual void Unmap() = 0;
	virtual void FlushMappedRange(size_t Size = RHI_WHOLE_SIZE, size_t Offset = 0u) = 0;
	virtual void InvalidateMappedRange(size_t Size = RHI_WHOLE_SIZE, size_t Offset = 0u) = 0;
	virtual bool Update(const void* Data, size_t Size, size_t SrcOffset = 0u, size_t DstOffset = 0u) = 0;
	virtual RHIBufferPtr Suballocate(const RHIBufferCreateInfo&) { return nullptr; }
	void* GetMappedMemory() const { return m_MappedMemory; }
protected:
	void* m_MappedMemory = nullptr;
};

struct RHIFrameBufferCreateInfo
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

	std::string Name;

	inline uint32_t GetNumColorAttachments() const { return NumColorAttachments; }

	inline bool HasDepthStencil() const { return DepthStencilAttachment.Texture != nullptr && (RHI::IsDepth(DepthStencilAttachment.Texture->GetFormat()) || RHI::IsDepthStencil(DepthStencilAttachment.Texture->GetFormat())); }

	inline RHIFrameBufferCreateInfo& AddColorAttachment(const RHITexture* Texture, RHISubresource Subresource = RHI::AllSubresource)
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

	inline RHIFrameBufferCreateInfo& SetColorAttachment(uint32_t Index, const RHITexture* Texture, RHISubresource Subresource = RHI::AllSubresource)
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

	inline RHIFrameBufferCreateInfo& SetDepthStencilAttachment(const RHITexture* Texture, RHISubresource Subresource = RHI::AllSubresource)
	{
		assert(Texture && (RHI::IsDepth(Texture->GetFormat()) || RHI::IsDepthStencil(Texture->GetFormat())));

		DepthStencilAttachment.Texture = Texture;
		DepthStencilAttachment.Subresource = Subresource;

		SetWidth(DepthStencilAttachment.GetWidth());
		SetHeight(DepthStencilAttachment.GetHeight());
		SetArrayLayers(DepthStencilAttachment.GetArrayLayers());

		return *this;
	}

	inline RHIFrameBufferCreateInfo& AddAttachment(const RHITexture* Texture, RHISubresource Subresource = RHI::AllSubresource)
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

	template<class T>
	inline RHIFrameBufferCreateInfo& SetName(T&& InName) { Name = std::move(std::string(std::forward<T>(InName))); return *this; }
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
	RHIFrameBuffer(const RHIFrameBufferCreateInfo& CreateInfo)
		: RHIResource(CreateInfo.Name.c_str())
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
