#pragma once

#include "Colorful/Vulkan/VulkanLoader.h"

NAMESPACE_START(RHI)

class VulkanShader final : public VkHWObject<IShader, VkShaderModule_T>
{
public:
	VulkanShader(class VulkanDevice* Device, const ShaderDesc& Desc);

	~VulkanShader();
};

class VulkanInputLayout final : public IInputLayout
{
public:
	VulkanInputLayout(const InputLayoutDesc& Desc, const ShaderDesc& VertexShaderDesc);

	VkPipelineVertexInputStateCreateInfo InputStateCreateInfo() const
	{
		assert(m_Bindings.size() && m_Attrs.size());

		return VkPipelineVertexInputStateCreateInfo
		{
			VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			nullptr,
			0u,
			static_cast<uint32_t>(m_Bindings.size()),
			m_Bindings.data(),
			static_cast<uint32_t>(m_Attrs.size()),
			m_Attrs.data()
		};
	}
protected:
private:
	std::vector<VkVertexInputBindingDescription> m_Bindings;
	std::vector<VkVertexInputAttributeDescription> m_Attrs;
};

NAMESPACE_END(RHI)