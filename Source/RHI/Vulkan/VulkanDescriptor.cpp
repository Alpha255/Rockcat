#include "RHI/Vulkan/VulkanDescriptor.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "RHI/Vulkan/VulkanDevelopSettings.h"

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
	auto& PhysicalDeviceLimits = Device.GetPhysicalDeviceLimits();
	auto& DevelopSettings = Device.GetDevelopSettings();

	auto GetPhysicalDeviceMaxNumDescriptor = [&PhysicalDeviceLimits](vk::DescriptorType DescriptorType) -> uint32_t {
		switch (DescriptorType)
		{
			case vk::DescriptorType::eSampler: return PhysicalDeviceLimits.maxPerStageDescriptorSamplers;
			case vk::DescriptorType::eCombinedImageSampler: return PhysicalDeviceLimits.maxDescriptorSetSampledImages;
			case vk::DescriptorType::eSampledImage: return PhysicalDeviceLimits.maxDescriptorSetSampledImages;
			case vk::DescriptorType::eStorageImage: return PhysicalDeviceLimits.maxDescriptorSetStorageImages;
			case vk::DescriptorType::eUniformTexelBuffer: return PhysicalDeviceLimits.maxDescriptorSetUniformBuffers;
			case vk::DescriptorType::eStorageTexelBuffer: return PhysicalDeviceLimits.maxDescriptorSetStorageBuffers;
			case vk::DescriptorType::eUniformBuffer: return PhysicalDeviceLimits.maxDescriptorSetUniformBuffers;
			case vk::DescriptorType::eStorageBuffer: return PhysicalDeviceLimits.maxDescriptorSetStorageBuffers;
			case vk::DescriptorType::eUniformBufferDynamic: return PhysicalDeviceLimits.maxDescriptorSetUniformBuffersDynamic;
			case vk::DescriptorType::eStorageBufferDynamic: return PhysicalDeviceLimits.maxDescriptorSetStorageBuffersDynamic;
			case vk::DescriptorType::eInputAttachment: return PhysicalDeviceLimits.maxDescriptorSetInputAttachments;
			default: return 0u; // Unsupported descriptor type
		}
	};

	for (auto DescriptorType = vk::DescriptorType::eSampler; DescriptorType <= vk::DescriptorType::eInputAttachment;)
	{
		assert(DevelopSettings.GetMaxNumDescriptor(DescriptorType) <= GetPhysicalDeviceMaxNumDescriptor(DescriptorType));
		DescriptorType = static_cast<vk::DescriptorType>(static_cast<size_t>(DescriptorType) + 1u);
	}
#endif

	std::vector<vk::DescriptorPoolSize> DescriptorPoolSizes(static_cast<size_t>(vk::DescriptorType::eInputAttachment) + 1u);
	for (auto DescriptorType = vk::DescriptorType::eSampler; DescriptorType <= vk::DescriptorType::eInputAttachment;)
	{
		DescriptorPoolSizes[static_cast<size_t>(DescriptorType)]
			.setDescriptorCount(DevelopSettings.GetMaxNumDescriptor(DescriptorType))
			.setType(DescriptorType);

		DescriptorType = static_cast<vk::DescriptorType>(static_cast<size_t>(DescriptorType) + 1u);
	}

	vk::DescriptorPoolCreateInfo CreateInfo;
	CreateInfo.setPoolSizes(DescriptorPoolSizes)
		.setMaxSets(DevelopSettings.DescriptorLimits.MaxDescriptorSetsPerPool);
	
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
	return m_AllocatedCount >= GetDevice().GetDevelopSettings().DescriptorLimits.MaxDescriptorSetsPerPool;
}
