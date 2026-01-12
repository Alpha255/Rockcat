#include "RHI/Vulkan/VulkanShader.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Asset/Shader.h"

VulkanShader::VulkanShader(const VulkanDevice& Device, const RHIShaderDesc& Desc)
	: VkHwResource(Device)
	, RHIShader(Desc)
{
	// #TODO: thread safe ???
	assert(Desc.Binary && Desc.Binary->GetBlob().IsValid() && (Desc.Binary->GetBlob().Size % sizeof(uint32_t) == 0));

	vk::ShaderModuleCreateInfo CreateInfo;
	CreateInfo.setCodeSize(Desc.Binary->GetBlob().Size)
		.setPCode(reinterpret_cast<const uint32_t*>(Desc.Binary->GetBlob().Data.get()));

	VERIFY_VK(GetNativeDevice().createShaderModule(&CreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

	VkHwResource::SetObjectName(Desc.Name.Get().data());
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