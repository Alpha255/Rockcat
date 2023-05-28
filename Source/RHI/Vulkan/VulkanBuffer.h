#pragma once

#include "Colorful/Vulkan/VulkanInstance.h"

NAMESPACE_START(RHI)

struct VkStatedObject
{
	EResourceState CurrentState = EResourceState::Common;
	EResourceState RequiredState = EResourceState::Common;
	EResourceState PermanentState = EResourceState::Unknown;
};

struct VulkanDeviceMemory
{
	VkDeviceMemory Handle = VK_NULL_HANDLE;
	size_t Size = 0u;

	bool8_t IsCoherent = false;
	bool8_t IsHostVisible = false;
	bool8_t IsHostCached = false;
};

class VulkanBuffer final : public VkHWObject<IBuffer, VkBuffer_T>, protected VkStatedObject
{
public:
	static_assert(WHOLE_SIZE == VK_WHOLE_SIZE, "Guarante the same value");

	VulkanBuffer(class VulkanDevice* Device, const BufferDesc& Desc);

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

	bool8_t Update(const void* Data, size_t Size, size_t SrcOffset, size_t DstOffset) override final;

	size_t Size() const
	{
		return m_DeviceMemory.Size;
	}

	bool8_t IsCoherent() const
	{
		return m_DeviceMemory.IsCoherent;
	}

	bool8_t IsHostVisible() const
	{
		return m_DeviceMemory.IsHostVisible;
	}

	bool8_t IsHostCached() const
	{
		return m_DeviceMemory.IsHostCached;
	}
protected:
	friend struct VulkanPipelineBarrier;
	friend class VulkanCommandBuffer;
	friend class VulkanGraphicsPipelineState;
private:
	VulkanDeviceMemory m_DeviceMemory;
};

NAMESPACE_END(RHI)