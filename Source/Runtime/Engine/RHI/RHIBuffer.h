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
	void* GetMappedMemory() const { return m_MappedMemory; }
protected:
	void* m_MappedMemory = nullptr;
};

struct RHIFrameBufferCreateInfo
{
	uint32_t Width = 0u;
	uint32_t Height = 0u;
	uint32_t ArrayLayers = 0u;
	std::string Name;

	const RHITexture* DepthStencilAttachment = nullptr;
	std::vector<const RHITexture*> ColorAttachments;

	inline RHIFrameBufferCreateInfo& SetWidth(uint32_t InWidth) { Width = InWidth; }
	inline RHIFrameBufferCreateInfo& SetHeight(uint32_t InHeight) { Height = InHeight; }
	inline RHIFrameBufferCreateInfo& SetArrayLayers(uint32_t InLayers) { ArrayLayers = InLayers; }

	inline RHIFrameBufferCreateInfo& AddColorAttachment(const RHITexture* Color)
	{
		assert(ColorAttachments.size() < ERHILimitations::MaxRenderTargets && Color && RHI::IsColor(Color->GetFormat()));
		ColorAttachments.emplace_back(Color);
		return *this;
	}

	inline RHIFrameBufferCreateInfo& SetColorAttachment(uint32_t Index, const RHITexture* Color)
	{
		assert(Index < ERHILimitations::MaxRenderTargets && Color && RHI::IsColor(Color->GetFormat()));
		ColorAttachments[Index] = Color;
		return *this;
	}

	inline RHIFrameBufferCreateInfo& SetDepthStencilAttachment(const RHITexture* DepthStencil)
	{
		assert(DepthStencil && RHI::IsDepthStenci(DepthStencil->GetFormat()));
		DepthStencilAttachment = DepthStencil;
		return *this;
	}

	template<class T>
	inline RHIFrameBufferCreateInfo& SetName(T&& InName) { Name = std::move(std::string(std::forward<T>(InName))); return *this; }
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
