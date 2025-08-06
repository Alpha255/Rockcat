#include "RHI/Vulkan/VulkanShader.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Asset/Shader.h"

VulkanShader::VulkanShader(const VulkanDevice& Device, const RHIShaderDesc& Desc)
	: VkHwResource(Device)
	, RHIShader(Desc)
{
	assert(Desc.Binary && Desc.Binary->IsValid() && Desc.Binary->GetSize() % sizeof(uint32_t) == 0);

	vk::ShaderModuleCreateInfo ShaderCreateInfo;
	ShaderCreateInfo.setCodeSize(Desc.Binary->GetSize())
		.setPCode(reinterpret_cast<const uint32_t*>(Desc.Binary->GetData()));

	VERIFY_VK(GetNativeDevice().createShaderModule(&ShaderCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

	VkHwResource::SetObjectName(Desc.Name.c_str());
}

VulkanInputLayout::VulkanInputLayout(const RHIInputLayoutDesc& Desc)
{
	for (uint32_t Index = 0u; Index < Desc.Bindings.size(); ++Index)
	{
		auto& BindingInfo = Desc.Bindings[Index];

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