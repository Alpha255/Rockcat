#pragma once

#include "RHI/Vulkan/VulkanTypes.h"

class VulkanShader final : public VkHwResource<vk::ShaderModule>
{
public:
	VulkanShader(const class VulkanDevice& Device, const RHIShaderCreateInfo& CreateInfo);
};

class VulkanInputLayout final : public RHIInputLayout
{
public:
	VulkanInputLayout(const RHIInputLayoutCreateInfo& CreateInfo);

#if 0
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
private:
	std::vector<VkVertexInputBindingDescription> m_Bindings;
	std::vector<VkVertexInputAttributeDescription> m_Attrs;
#endif
};