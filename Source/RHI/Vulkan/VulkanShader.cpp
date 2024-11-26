#include "RHI/Vulkan/VulkanShader.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Engine/Asset/ShaderAsset.h"

VulkanShader::VulkanShader(const VulkanDevice& Device, const RHIShaderCreateInfo& CreateInfo)
	: VkHwResource(Device)
	, RHIShader(CreateInfo)
{
	assert(CreateInfo.Binary && CreateInfo.Binary->IsValid() && CreateInfo.Binary->GetSize() % sizeof(uint32_t) == 0);

	vk::ShaderModuleCreateInfo ShaderCreateInfo;
	ShaderCreateInfo.setCodeSize(CreateInfo.Binary->GetSize())
		.setPCode(reinterpret_cast<const uint32_t*>(CreateInfo.Binary->GetBinary()));

	VERIFY_VK(GetNativeDevice().createShaderModule(&ShaderCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

	VkHwResource::SetObjectName(CreateInfo.Name.c_str());
}

VulkanInputLayout::VulkanInputLayout(const RHIInputLayoutCreateInfo& CreateInfo)
{
	for (uint32_t Index = 0u; Index < CreateInfo.Bindings.size(); ++Index)
	{
		auto& BindingInfo = CreateInfo.Bindings[Index];

		m_InputBindings.emplace_back(vk::VertexInputBindingDescription())
			.setBinding(BindingInfo.Binding)
			.setStride(BindingInfo.Stride)
			.setInputRate(GetInputRate(BindingInfo.InputRate));

		uint32_t Offset = 0u;
		for (auto& AttributeInfo : BindingInfo.Attributes)
		{
			m_InputAttributes.emplace_back(vk::VertexInputAttributeDescription())
				.setLocation(AttributeInfo.Location)
				.setFormat(GetFormat(AttributeInfo.Format))
				.setOffset(Offset)
				.setBinding(Index);

			Offset += AttributeInfo.Stride;
		}
	}

	vk::PipelineVertexInputStateCreateInfo::setVertexBindingDescriptions(m_InputBindings)
		.setVertexAttributeDescriptions(m_InputAttributes);
}