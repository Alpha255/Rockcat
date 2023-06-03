#include "RHI/Vulkan/VulkanShader.h"
#include "RHI/Vulkan/VulkanDevice.h"
#include "Runtime/Engine/Asset/ShaderAsset.h"

VulkanShader::VulkanShader(const class VulkanDevice& Device, const RHIShaderCreateInfo& CreateInfo)
	: VkDeviceResource(Device)
{
	assert(GetStage() < ERHIShaderStage::Num && CreateInfo.Binary);
	assert(CreateInfo.Binary->GetSize() && CreateInfo.Binary->GetSize() % sizeof(uint32_t) == 0);

	auto vkCreateInfo = vk::ShaderModuleCreateInfo()
		.setCodeSize(CreateInfo.Binary->GetSize())
		.setPCode(reinterpret_cast<const uint32_t*>(CreateInfo.Binary->GetBinary()));

	VERIFY_VK(GetNativeDevice().createShaderModule(&vkCreateInfo, nullptr, &m_Shader));

	SetDebugName(CreateInfo.Name.c_str());
}

VulkanShader::~VulkanShader()
{
	GetNativeDevice().destroy(m_Shader);
	m_Shader = nullptr;
}

void VulkanShader::SetDebugName(const char8_t* Name)
{
	assert(Name);
	GetDevice().SetObjectName(m_Shader, Name);
	RHIShader::SetDebugName(Name);
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