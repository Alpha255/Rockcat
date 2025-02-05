#include "RHI/Vulkan/VulkanEnvConfiguration.h"

std::vector<uint32_t> VulkanEnvConfiguration::GetDescriptorLimitationList() const
{
	std::vector<uint32_t> Limitations((size_t)vk::DescriptorType::eInputAttachment + 1u);
	Limitations[(size_t)vk::DescriptorType::eSampler] = DescriptorLimitationConfigs.MaxSampler;
	Limitations[(size_t)vk::DescriptorType::eCombinedImageSampler] = DescriptorLimitationConfigs.MaxCombinedImageSampler;
	Limitations[(size_t)vk::DescriptorType::eSampledImage] = DescriptorLimitationConfigs.MaxSampledImage;
	Limitations[(size_t)vk::DescriptorType::eStorageImage] = DescriptorLimitationConfigs.MaxStorageImage;
	Limitations[(size_t)vk::DescriptorType::eUniformTexelBuffer] = DescriptorLimitationConfigs.MaxUniformTexelBuffer;
	Limitations[(size_t)vk::DescriptorType::eStorageTexelBuffer] = DescriptorLimitationConfigs.MaxStorageTexelBuffer;
	Limitations[(size_t)vk::DescriptorType::eUniformBuffer] = DescriptorLimitationConfigs.MaxUniformBuffer;
	Limitations[(size_t)vk::DescriptorType::eStorageBuffer] = DescriptorLimitationConfigs.MaxStorageBuffer;
	Limitations[(size_t)vk::DescriptorType::eUniformBufferDynamic] = DescriptorLimitationConfigs.MaxUniformBufferDynamic;
	Limitations[(size_t)vk::DescriptorType::eStorageBufferDynamic] = DescriptorLimitationConfigs.MaxStorageBufferDynamic;
	Limitations[(size_t)vk::DescriptorType::eInputAttachment] = DescriptorLimitationConfigs.MaxInputAttachment;
	return Limitations;
}
