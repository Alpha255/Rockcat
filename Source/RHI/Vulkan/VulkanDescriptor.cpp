#include "RHI/Vulkan/VulkanDescriptor.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Runtime/Engine/Engine.h"

static constexpr uint32_t DescriptorPoolLimits[] =
{
	256u,  DESCRIPTION(VK_DESCRIPTOR_TYPE_SAMPLER)
	2048u, DESCRIPTION(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
	2048u, DESCRIPTION(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)
	256u,  DESCRIPTION(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
	256u,  DESCRIPTION(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER)
	256u,  DESCRIPTION(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER)
	1024u, DESCRIPTION(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
	1024u, DESCRIPTION(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)
	8u,    DESCRIPTION(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
	8u,    DESCRIPTION(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
	64u,   DESCRIPTION(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
};

static constexpr uint32_t DescriptorSetsLimit = 2048u;

VulkanDescriptorPool::VulkanDescriptorPool(const VulkanDevice& Device)
	: VkHwResource(Device)
{
#if _DEBUG
	auto& DeviceLimits = GetDevice().GetPhysicalDeviceLimits();
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

	/// assert(DescriptorSetsLimit <= DeviceLimits.maxBoundDescriptorSets);

	for (auto DescriptorIndex = vk::DescriptorType::eSampler; DescriptorIndex <= vk::DescriptorType::eInputAttachment;)
	{
		assert(DescriptorPoolLimits[static_cast<size_t>(DescriptorIndex)] <= DeviceDescriptorLimits[static_cast<size_t>(DescriptorIndex)]);
		DescriptorIndex = static_cast<vk::DescriptorType>(static_cast<size_t>(DescriptorIndex) + 1u);
	}
#endif

	std::vector<vk::DescriptorPoolSize> DescriptorPoolSizes(static_cast<size_t>(vk::DescriptorType::eInputAttachment) + 1u);
	for (auto DescriptorIndex = vk::DescriptorType::eSampler; DescriptorIndex <= vk::DescriptorType::eInputAttachment;)
	{
		DescriptorPoolSizes[static_cast<size_t>(DescriptorIndex)] = vk::DescriptorPoolSize()
			.setDescriptorCount(DescriptorPoolLimits[static_cast<size_t>(DescriptorIndex)])
			.setType(DescriptorIndex);
		DescriptorIndex = static_cast<vk::DescriptorType>(static_cast<size_t>(DescriptorIndex) + 1u);
	}

	auto vkCreateInfo = vk::DescriptorPoolCreateInfo()
		.setPoolSizes(DescriptorPoolSizes)
		.setMaxSets(DescriptorSetsLimit);
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

bool8_t VulkanDescriptorPool::IsFull() const
{
	return m_AllocatedCount == DescriptorSetsLimit;
}

#if 0
VulkanDescriptor::KeyBindings VulkanDescriptor::MakeKeyBindings(VulkanDevice* Device, const GraphicsPipelineDesc& Desc)
{
	size_t Hash = 0u;
	std::vector<VkDescriptorSetLayoutBinding> Bindings;
	std::vector<VkPushConstantRange> PushConstantRanges;

	Desc.Shaders.ForEach([Device, &Hash, &Bindings, &PushConstantRanges](const IShader* Shader) {
			/// Desc.Shaders[i]->Desc()->Binary.clear();  #TODO Clear shader binary here ???
			assert(Shader->Desc());

			for (auto& Variable : Shader->Desc()->Variables)
			{
				if (Variable.Type == EResourceType::PushConstants)
				{
					assert(Variable.ConstantsSize <= Device->PhysicalLimits().maxPushConstantsSize);

					PushConstantRanges.emplace_back(
						VkPushConstantRange
						{
							VkType::ShaderStage(Shader->Stage()) | 0u,
							0u,
							Variable.ConstantsSize
						}
					);

					HashCombine(Hash, Shader->Stage(), EResourceType::PushConstants, Variable.ConstantsSize);
				}
				else
				{
					VkDescriptorSetLayoutBinding Binding
					{
						Variable.Binding,
						VkType::DescriptorType(Variable.Type),
						Variable.Type == EResourceType::PushConstants ? 0u : 1u, /// #TODO Handle resource array
						VkType::ShaderStage(Shader->Stage()) | 0u,               /// #TODO Support multi shader stages
						nullptr
					};

					HashCombine(Hash, Binding.stageFlags, Binding.descriptorType, Binding.binding);

					Bindings.emplace_back(Binding);
				}
			}
		});

	return std::make_tuple(Hash, Bindings, PushConstantRanges);
}

#endif
