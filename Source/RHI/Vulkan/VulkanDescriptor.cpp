#include "RHI/Vulkan/VulkanDescriptor.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Engine/Services/SpdLogService.h"

//static constexpr uint32_t DescriptorPoolLimits[] =
//{
//	256u,  DESCRIPTION(VK_DESCRIPTOR_TYPE_SAMPLER)
//	2048u, DESCRIPTION(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
//	2048u, DESCRIPTION(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
//	256u,  DESCRIPTION(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
//	256u,  DESCRIPTION(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER)
//	256u,  DESCRIPTION(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER)
//	1024u, DESCRIPTION(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
//	1024u, DESCRIPTION(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
//	8u,    DESCRIPTION(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
//	8u,    DESCRIPTION(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
//	64u,   DESCRIPTION(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
//};
//
//static constexpr uint32_t DescriptorSetsLimit = 2048u;

std::vector<uint32_t> VulkanDescriptorLimitations::GetLimitationList() const
{
	std::vector<uint32_t> Limitations((size_t)vk::DescriptorType::eInputAttachment + 1u);
	Limitations[(size_t)vk::DescriptorType::eSampler] = MaxSampler;
	Limitations[(size_t)vk::DescriptorType::eCombinedImageSampler] = MaxCombinedImageSampler;
	Limitations[(size_t)vk::DescriptorType::eSampledImage] = MaxSampledImage;
	Limitations[(size_t)vk::DescriptorType::eStorageImage] = MaxStorageImage;
	Limitations[(size_t)vk::DescriptorType::eUniformTexelBuffer] = MaxUniformTexelBuffer;
	Limitations[(size_t)vk::DescriptorType::eStorageTexelBuffer] = MaxStorageTexelBuffer;
	Limitations[(size_t)vk::DescriptorType::eUniformBuffer] = MaxUniformBuffer;
	Limitations[(size_t)vk::DescriptorType::eStorageBuffer] = MaxStorageBuffer;
	Limitations[(size_t)vk::DescriptorType::eUniformBufferDynamic] = MaxUniformBufferDynamic;
	Limitations[(size_t)vk::DescriptorType::eStorageBufferDynamic] = MaxStorageBufferDynamic;
	Limitations[(size_t)vk::DescriptorType::eInputAttachment] = MaxInputAttachment;
	return Limitations;
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanDevice& Device, const RHIShaderResourceBindings& CreateInfo)
	: VkHwResource(Device)
{
	std::vector<vk::DescriptorSetLayoutBinding> Bindings(CreateInfo.size());
	for (uint32_t Index = 0u; Index < CreateInfo.size(); ++Index)
	{
		Bindings[Index].setBinding(CreateInfo[Index].Binding)
			.setDescriptorType(GetDescriptorType(CreateInfo[Index].Type))
			.setDescriptorCount(1u)
			.setStageFlags(GetShaderStageFlags(CreateInfo[Index].Stage))
			.setPImmutableSamplers(nullptr);
	}

	auto vkCreateInfo = vk::DescriptorSetLayoutCreateInfo()
		.setBindings(Bindings);
	VERIFY_VK(GetNativeDevice().createDescriptorSetLayout(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

VulkanPipelineLayout::VulkanPipelineLayout(const VulkanDevice& Device, const RHIShaderResourceBindings& CreateInfo)
	: VkHwResource(Device)
	, m_DescriptorSetLayout(Device, CreateInfo)
{
	auto vkCreateInfo = vk::PipelineLayoutCreateInfo()
		.setSetLayoutCount(1u)
		.setPSetLayouts(&m_DescriptorSetLayout.GetNative()); // TODO: PushConstants
	VERIFY_VK(GetNativeDevice().createPipelineLayout(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
}

std::shared_ptr<VulkanDescriptorLimitations> VulkanDescriptorPool::s_DescriptorLimitations;

VulkanDescriptorPool::VulkanDescriptorPool(const VulkanDevice& Device)
	: VkHwResource(Device)
{
	if (!s_DescriptorLimitations)
	{
		s_DescriptorLimitations = VulkanDescriptorLimitations::Load("Configs\\VkDescriptorLimitations.json");
		assert(s_DescriptorLimitations);
	}

#if _DEBUG
	auto& DeviceLimits = GetDevice().GetPhysicalDeviceLimits();
	auto ConfigLimits = s_DescriptorLimitations->GetLimitationList();

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
		DescriptorPoolSizes[static_cast<size_t>(DescriptorIndex)] = vk::DescriptorPoolSize()
			.setDescriptorCount(ConfigLimits[static_cast<size_t>(DescriptorIndex)])
			.setType(DescriptorIndex);
		DescriptorIndex = static_cast<vk::DescriptorType>(static_cast<size_t>(DescriptorIndex) + 1u);
	}

	auto vkCreateInfo = vk::DescriptorPoolCreateInfo()
		.setPoolSizes(DescriptorPoolSizes)
		.setMaxSets(s_DescriptorLimitations->MaxDescriptorSetsPerPool);
	VERIFY_VK(GetNativeDevice().createDescriptorPool(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));
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

vk::DescriptorSet VulkanDescriptorPool::Alloc(vk::DescriptorSetLayout DescriptorSetLayout)
{
	assert(!IsFull() && DescriptorSetLayout);

	auto AllocInfo = vk::DescriptorSetAllocateInfo()
		.setDescriptorPool(m_Native)
		.setPSetLayouts(&DescriptorSetLayout)
		.setDescriptorSetCount(1u);

	vk::DescriptorSet DescriptorSet;
	VERIFY_VK(GetNativeDevice().allocateDescriptorSets(&AllocInfo, &DescriptorSet));
	++m_AllocatedCount;
	return DescriptorSet;
}

bool VulkanDescriptorPool::IsFull() const
{
	return m_AllocatedCount >= s_DescriptorLimitations->MaxDescriptorSetsPerPool;
}

VulkanDescriptorSet::VulkanDescriptorSet(const class VulkanDevice& Device, vk::PipelineLayout PipelineLayout, vk::DescriptorSetLayout DescriptorSetLayout, vk::DescriptorSet Set)
	: VkDeviceResource(Device)
	, m_PipelineLayout(PipelineLayout)
	, m_DescriptorSetLayout(DescriptorSetLayout)
{
	m_Native = Set;
}

void VulkanDescriptorSet::Commit(const RHIShaderResourceBindings& Bindings)
{
	std::vector<vk::DescriptorBufferInfo> Buffers;
	std::vector<vk::DescriptorImageInfo> Images;
	std::vector<vk::WriteDescriptorSet> Writes;

	for (auto& Variable : Bindings)
	{
		assert(Variable.Resource);

		auto vkWrite = vk::WriteDescriptorSet()
			.setDstSet(GetNative())
			.setDescriptorCount(1u)
			.setDescriptorType(GetDescriptorType(Variable.Type))
			.setDstBinding(Variable.Binding);

		switch (Variable.Type)
		{
		case ERHIResourceType::UniformTexelBuffer:
		case ERHIResourceType::StorageTexelBuffer:
		case ERHIResourceType::UniformBuffer:
		case ERHIResourceType::StorageBuffer:
		case ERHIResourceType::UniformBufferDynamic:
		case ERHIResourceType::StorageBufferDynamic:
			Buffers.emplace_back(vk::DescriptorBufferInfo()
				.setBuffer(Cast<VulkanBuffer>(Variable.Resource)->GetNative())
				.setOffset(0u)
				.setRange(0u));
			vkWrite.setPBufferInfo(&Buffers.back());
			break;
		case ERHIResourceType::SampledImage:
		case ERHIResourceType::StorageImage:
			Images.emplace_back(vk::DescriptorImageInfo()
				.setImageView(nullptr)
				.setImageLayout(vk::ImageLayout::eUndefined)
				.setSampler(nullptr));
			vkWrite.setPImageInfo(&Images.back());
			break;
		case ERHIResourceType::CombinedImageSampler:
			Images.emplace_back(vk::DescriptorImageInfo()
				.setImageView(nullptr)
				.setImageLayout(vk::ImageLayout::eUndefined)
				.setSampler(nullptr));
			vkWrite.setPImageInfo(&Images.back());
			break;
		case ERHIResourceType::Sampler:
			Images.emplace_back(vk::DescriptorImageInfo()
				.setImageView(nullptr)
				.setImageLayout(vk::ImageLayout::eUndefined)
				.setSampler(Cast<VulkanSampler>(Variable.Resource)->GetNative()));
			vkWrite.setPImageInfo(&Images.back());
			break;
		default:
			assert(false);
			break;
		}
		Writes.emplace_back(std::move(vkWrite));
	}

	GetNativeDevice().updateDescriptorSets(static_cast<uint32_t>(Writes.size()), Writes.data(), 0u, nullptr);
}
