#include "Colorful/Vulkan/VulkanShader.h"
#include "Colorful/Vulkan/VulkanDevice.h"

NAMESPACE_START(RHI)

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

VulkanShader::VulkanShader(VulkanDevice* Device, const ShaderDesc& Desc)
	: VkHWObject(Device, Desc)
{
	assert(m_Stage < EShaderStage::ShaderStageCount && Desc.BinarySize);

	VkShaderModuleCreateInfo CreateInfo
	{
		VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		nullptr,
		0u,
		Desc.BinarySize,
		Desc.Binary.data()
	};

	VERIFY_VK(vkCreateShaderModule(m_Device->Get(), &CreateInfo, VK_ALLOCATION_CALLBACKS, Reference()));
}

VulkanShader::~VulkanShader()
{
	vkDestroyShaderModule(m_Device->Get(), Get(), VK_ALLOCATION_CALLBACKS);
}

NAMESPACE_END(RHI)