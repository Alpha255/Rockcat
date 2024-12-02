#pragma once

#include "RHI/Vulkan/VulkanTexture.h"
#include "Engine/Asset/SerializableAsset.h"

/// VulkanDescriptor Manager???

struct VulkanDescriptorLimitations : public SerializableAsset<VulkanDescriptorLimitations>
{
	using BaseClass::BaseClass;

	uint32_t MaxDescriptorSetsPerPool;

	uint32_t MaxSampler;
	uint32_t MaxCombinedImageSampler;
	uint32_t MaxSampledImage;
	uint32_t MaxStorageImage;
	uint32_t MaxUniformTexelBuffer;
	uint32_t MaxStorageTexelBuffer;
	uint32_t MaxUniformBuffer;
	uint32_t MaxStorageBuffer;
	uint32_t MaxUniformBufferDynamic;
	uint32_t MaxStorageBufferDynamic;
	uint32_t MaxInputAttachment;

	std::vector<uint32_t> GetLimitationList() const;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(MaxDescriptorSetsPerPool),
			CEREAL_NVP(MaxSampler),
			CEREAL_NVP(MaxCombinedImageSampler),
			CEREAL_NVP(MaxSampledImage),
			CEREAL_NVP(MaxStorageImage),
			CEREAL_NVP(MaxUniformTexelBuffer),
			CEREAL_NVP(MaxStorageTexelBuffer),
			CEREAL_NVP(MaxUniformBuffer),
			CEREAL_NVP(MaxStorageBuffer),
			CEREAL_NVP(MaxUniformBufferDynamic),
			CEREAL_NVP(MaxStorageBufferDynamic),
			CEREAL_NVP(MaxInputAttachment)
		);
	}
};

class VulkanDescriptorSetLayout final : public VkHwResource<vk::DescriptorSetLayout>
{
public:
	VulkanDescriptorSetLayout(const class VulkanDevice& Device, const RHIShaderResourceLayout& LayoutDesc);
};

class VulkanPipelineLayout final : public VkHwResource<vk::PipelineLayout>
{
public:
	VulkanPipelineLayout(const class VulkanDevice& Device, const vk::DescriptorSetLayout& DescriptorSetLayout);
};

class VulkanDescriptorPool final : public VkHwResource<vk::DescriptorPool>
{
public:
	VulkanDescriptorPool(const class VulkanDevice& Device);

	~VulkanDescriptorPool() = default;

	void Reset();

	vk::DescriptorSet Allocate(vk::DescriptorSetLayout DescriptorSetLayout);
	void Free(vk::DescriptorSet Set);

	bool IsFull() const;
protected:
private:
	uint32_t m_AllocatedCount = 0u;

	static std::shared_ptr<VulkanDescriptorLimitations> s_DescriptorLimitations;
};