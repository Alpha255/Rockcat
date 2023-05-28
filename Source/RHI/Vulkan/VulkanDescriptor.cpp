#include "Colorful/Vulkan/VulkanDescriptor.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(RHI)

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

VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice* Device)
	: VkHWObject(Device)
{
#if _DEBUG
	auto& DeviceLimits = m_Device->PhysicalLimits();
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

	for (uint32_t DescriptorIndex = VK_DESCRIPTOR_TYPE_SAMPLER; DescriptorIndex <= VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; ++DescriptorIndex)
	{
		assert(DescriptorPoolLimits[DescriptorIndex] <= DeviceDescriptorLimits[DescriptorIndex]);
	}
#endif

	std::vector<VkDescriptorPoolSize> DescriptorPoolSizes(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT + 1u);
	for (uint32_t DescriptorIndex = VK_DESCRIPTOR_TYPE_SAMPLER; DescriptorIndex <= VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; ++DescriptorIndex)
	{
		DescriptorPoolSizes[DescriptorIndex] = VkDescriptorPoolSize
		{
			static_cast<VkDescriptorType>(DescriptorIndex),
			DescriptorPoolLimits[DescriptorIndex]
		};
	}

	VkDescriptorPoolCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		nullptr,
		0u,
		DescriptorSetsLimit,
		static_cast<uint32_t>(DescriptorPoolSizes.size()),
		DescriptorPoolSizes.data()
	};
	VERIFY_VK(vkCreateDescriptorPool(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));
}

void VulkanDescriptorPool::Reset()
{
	/// return all descriptor sets allocated from a given pool to the pool
	/// flags is reserved for future use
	/// Resetting a descriptor pool recycles all of the resources from all of the descriptor sets allocated from the descriptor pool back to the descriptor pool, 
	/// and the descriptor sets are implicitly freed.
	assert(IsValid());
	vkResetDescriptorPool(m_Device->Get(), Get(), 0u);
}

VkDescriptorSet VulkanDescriptorPool::Alloc(VkDescriptorSetLayout DescriptorSetLayout)
{
	assert(!IsFull() && DescriptorSetLayout != VK_NULL_HANDLE);

	VkDescriptorSetAllocateInfo AllocInfo
	{
		VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		nullptr,
		Get(),
		1u,
		& DescriptorSetLayout
	};

	VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;
	VERIFY_VK(vkAllocateDescriptorSets(m_Device->Get(), &AllocInfo, &DescriptorSet));
	++m_AllocatedCount;
	return DescriptorSet;
}

bool8_t VulkanDescriptorPool::IsFull() const
{
	return m_AllocatedCount == DescriptorSetsLimit;
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
	/// When a pool is destroyed, all descriptor sets allocated from the pool are implicitly freed and become invalid. 
	/// Descriptor sets allocated from a given pool do not need to be freed before destroying that descriptor pool.
	vkDestroyDescriptorPool(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

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

NAMESPACE_END(RHI)
