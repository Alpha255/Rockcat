#include "RHI/Vulkan/VulkanShader.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Engine/Asset/ShaderAsset.h"

VulkanShader::VulkanShader(const VulkanDevice& Device, const RHIShaderCreateInfo& CreateInfo)
	: VkHwResource(Device)
{
	assert(CreateInfo.Binary && CreateInfo.Binary->GetSize() && CreateInfo.Binary->GetSize() % sizeof(uint32_t) == 0);

	auto vkCreateInfo = vk::ShaderModuleCreateInfo()
		.setCodeSize(CreateInfo.Binary->GetSize())
		.setPCode(reinterpret_cast<const uint32_t*>(CreateInfo.Binary->GetBinary()));

	VERIFY_VK(GetNativeDevice().createShaderModule(&vkCreateInfo, VK_ALLOCATION_CALLBACKS, &m_Native));

	VkHwResource::SetDebugName(CreateInfo.Name.c_str());
}

#if 0
VulkanInputLayout::VulkanInputLayout(const InputLayoutDesc& Desc, const ShaderDesc& VertexShaderDesc)
{
	(void)VertexShaderDesc;

	for (uint32_t BindingIndex = 0u; BindingIndex < Desc.Bindings.size(); ++BindingIndex)
	{
		VkVertexInputBindingDescription BindingDescription
		{
			Desc.Bindings[BindingIndex].Binding,
			Desc.Bindings[BindingIndex].Stride,
			Desc.Bindings[BindingIndex].InputRate == EVertexInputRate::Instance ?
				VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX
		};

		uint32_t Offset = 0u;
		for (auto& Attribute : Desc.Bindings[BindingIndex].Attributes)
		{
			VkVertexInputAttributeDescription AttributeDesc
			{
				Attribute.Location,
				BindingIndex,
				VkType::Format(Attribute.Format),
				Offset
			};
			m_Attrs.emplace_back(AttributeDesc);

			Offset += Attribute.Stride;
		}
		m_Bindings.emplace_back(BindingDescription);
	}
}
#endif