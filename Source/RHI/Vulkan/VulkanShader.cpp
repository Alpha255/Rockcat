#include "RHI/Vulkan/VulkanShader.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Engine/Asset/ShaderAsset.h"

VulkanShader::VulkanShader(const VulkanDevice& Device, const RHIShaderCreateInfo& CreateInfo)
	: VkHwResource(Device)
	, RHIShader(CreateInfo)
{
	assert(CreateInfo.Binary && CreateInfo.Binary->IsValid() && CreateInfo.Binary->GetSize() % sizeof(uint32_t) == 0);

	auto vkCreateInfo = vk::ShaderModuleCreateInfo()
		.setCodeSize(CreateInfo.Binary->GetSize())
		.setPCode(reinterpret_cast<const uint32_t*>(CreateInfo.Binary->GetBinary()));

	VERIFY_VK(GetNativeDevice().createShaderModule(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

	VkHwResource::SetObjectName(CreateInfo.Name.c_str());
}

VulkanInputLayout::VulkanInputLayout(const RHIInputLayoutCreateInfo& CreateInfo)
{
	for (uint32_t Index = 0u; Index < CreateInfo.Bindings.size(); ++Index)
	{
		auto& BindingInfo = CreateInfo.Bindings[Index];
		auto InputBinding = vk::VertexInputBindingDescription()
			.setBinding(BindingInfo.Binding)
			.setStride(BindingInfo.Stride)
			.setInputRate(GetInputRate(BindingInfo.InputRate));

		uint32_t Offset = 0u;
		for (auto& AttributeInfo : BindingInfo.Attributes)
		{
			auto InputAttribute = vk::VertexInputAttributeDescription()
				.setLocation(AttributeInfo.Location)
				.setFormat(GetFormat(AttributeInfo.Format))
				.setOffset(Offset)
				.setBinding(Index);

			Offset += AttributeInfo.Stride;

			m_InputAttributes.emplace_back(std::move(InputAttribute));
		}

		m_InputBindings.emplace_back(std::move(InputBinding));
	}

	vk::PipelineVertexInputStateCreateInfo::setVertexBindingDescriptions(m_InputBindings)
		.setVertexAttributeDescriptions(m_InputAttributes);
}