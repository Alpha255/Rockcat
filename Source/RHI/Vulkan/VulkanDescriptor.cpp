#include "RHI/Vulkan/VulkanDescriptor.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanEnvConfiguration.h"
#include "RHI/Vulkan/VulkanRHI.h"

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanDevice& Device, const RHIShaderResourceLayout& LayoutDesc)
	: VkHwResource(Device)
{
	std::vector<vk::DescriptorSetLayoutBinding> Bindings;
	for (uint32_t Index = 0u; Index < LayoutDesc.size(); ++Index)
	{
		auto Stage = static_cast<ERHIShaderStage>(Index);

		for (auto& ResourceInfo : LayoutDesc[Stage])
		{
			Bindings.emplace_back(vk::DescriptorSetLayoutBinding())
				.setBinding(ResourceInfo.Binding)
				.setDescriptorType(GetDescriptorType(ResourceInfo.Type))
				.setDescriptorCount(1u)
				.setStageFlags(GetShaderStageFlags(Stage))
				.setPImmutableSamplers(nullptr);
		}
	}

	vk::DescriptorSetLayoutCreateInfo CreateInfo;
	CreateInfo.setBindings(Bindings);

	VERIFY_VK(GetNativeDevice().createDescriptorSetLayout(&CreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

VulkanPipelineLayout::VulkanPipelineLayout(const VulkanDevice& Device, const vk::DescriptorSetLayout& DescriptorSetLayout)
	: VkHwResource(Device)
{
	vk::PipelineLayoutCreateInfo CreateInfo;
	CreateInfo.setSetLayoutCount(1u)
		.setPSetLayouts(&DescriptorSetLayout); // TODO: PushConstants
	
	VERIFY_VK(GetNativeDevice().createPipelineLayout(&CreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

VulkanDescriptorPool::VulkanDescriptorPool(const VulkanDevice& Device)
	: VkHwResource(Device)
{
#if _DEBUG
	auto& DeviceLimits = Device.GetPhysicalDeviceLimits();
	auto ConfigLimits = VulkanRHI::GetEnvConfigs().GetDescriptorLimitationList();

	const std::vector<uint32_t> DeviceDescriptorLimits = 
	{
		DeviceLimits.maxDescriptorSetSamplers,
		DeviceLimits.maxDescriptorSetSampledImages,
		DeviceLimits.maxDescriptorSetSampledImages,
		DeviceLimits.maxDescriptorSetStorageImages,
		DeviceLimits.maxDescriptorSetUniformBuffers,
		DeviceLimits.maxDescriptorSetStorageBuffers,
		DeviceLimits.maxDescriptorSetUniformBuffers,
		DeviceLimits.maxDescriptorSetStorageBuffers,
		DeviceLimits.maxDescriptorSetUniformBuffersDynamic,
		DeviceLimits.maxDescriptorSetStorageBuffersDynamic,
		DeviceLimits.maxDescriptorSetInputAttachments
	};

	for (auto DescriptorIndex = vk::DescriptorType::eSampler; DescriptorIndex <= vk::DescriptorType::eInputAttachment;)
	{
		assert(ConfigLimits[static_cast<size_t>(DescriptorIndex)] <= DeviceDescriptorLimits[static_cast<size_t>(DescriptorIndex)]);
		DescriptorIndex = static_cast<vk::DescriptorType>(static_cast<size_t>(DescriptorIndex) + 1u);
	}
#endif

	std::vector<vk::DescriptorPoolSize> DescriptorPoolSizes(static_cast<size_t>(vk::DescriptorType::eInputAttachment) + 1u);
	for (auto DescriptorIndex = vk::DescriptorType::eSampler; DescriptorIndex <= vk::DescriptorType::eInputAttachment;)
	{
		DescriptorPoolSizes[static_cast<size_t>(DescriptorIndex)]
			.setDescriptorCount(ConfigLimits[static_cast<size_t>(DescriptorIndex)])
			.setType(DescriptorIndex);

		DescriptorIndex = static_cast<vk::DescriptorType>(static_cast<size_t>(DescriptorIndex) + 1u);
	}

	vk::DescriptorPoolCreateInfo CreateInfo;
	CreateInfo.setPoolSizes(DescriptorPoolSizes)
		.setMaxSets(VulkanRHI::GetDescriptorLimitationConfigs().MaxDescriptorSetsPerPool);
	
	VERIFY_VK(GetNativeDevice().createDescriptorPool(&CreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

void VulkanDescriptorPool::Reset()
{
	/// return all descriptor sets allocated from a given pool to the pool
	/// flags is reserved for future use
	/// Resetting a descriptor pool recycles all of the resources from all of the descriptor sets allocated from the descriptor pool back to the descriptor pool, 
	/// and the descriptor sets are implicitly freed.
	assert(m_Native);
	GetNativeDevice().resetDescriptorPool(m_Native);
}

vk::DescriptorSet VulkanDescriptorPool::Allocate(vk::DescriptorSetLayout DescriptorSetLayout)
{
	assert(!IsFull() && DescriptorSetLayout);

	vk::DescriptorSetAllocateInfo AllocInfo;
	AllocInfo.setDescriptorPool(m_Native)
		.setPSetLayouts(&DescriptorSetLayout)
		.setDescriptorSetCount(1u);

	vk::DescriptorSet DescriptorSet;
	VERIFY_VK(GetNativeDevice().allocateDescriptorSets(&AllocInfo, &DescriptorSet));

	++m_AllocatedCount;
	return DescriptorSet;
}

void VulkanDescriptorPool::Free(vk::DescriptorSet Set)
{
	std::vector<vk::DescriptorSet> DescriptorSets{ Set };
	GetNativeDevice().freeDescriptorSets(m_Native, DescriptorSets);
	--m_AllocatedCount;
}

bool VulkanDescriptorPool::IsFull() const
{
	return m_AllocatedCount >= VulkanRHI::GetDescriptorLimitationConfigs().MaxDescriptorSetsPerPool;
}
