#pragma once

#include "RHI/Vulkan/VulkanTypes.h"

class VulkanBuffer final : public VkHwResource<vk::Buffer>, public RHIBuffer
{
public:
	VulkanBuffer(const class VulkanDevice& Device, const RHIBufferCreateInfo& CreateInfo);

	~VulkanBuffer();

	void* Map(size_t Size, size_t Offset) override final;

	void Unmap() override final;

	/// vkFlushMappedMemoryRanges guarantees that host writes to the memory ranges described by pMemoryRanges are made available to the host memory domain, 
	/// such that they can be made available to the device memory domain via memory domain operations using the VK_ACCESS_HOST_WRITE_BIT access type.
	void FlushMappedRange(size_t Size, size_t Offset) override final;

	/// vkInvalidateMappedMemoryRanges guarantees that device writes to the memory ranges described by pMemoryRanges, 
	/// which have been made available to the host memory domain using the VK_ACCESS_HOST_WRITE_BIT and VK_ACCESS_HOST_READ_BIT access types, 
	/// are made visible to the host. If a range of non-coherent memory is written by the host and then invalidated without first being flushed, its contents are undefined.
	void InvalidateMappedRange(size_t Size, size_t Offset) override final;

	bool Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset) override final;

	inline size_t GetSize() const { return m_Size; }
	inline bool IsCoherent() const { return m_Coherent; }
	inline bool IsHostVisible() const { return m_HostVisible; }
	bool IsHostCached() const { return m_HostCached; }
private:
	vk::DeviceMemory m_Memory;

	bool m_Coherent = false;
	bool m_HostVisible = false;
	bool m_HostCached = false;

	size_t m_Size = 0u;
};