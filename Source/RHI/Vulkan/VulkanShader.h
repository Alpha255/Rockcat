#pragma once

#include "RHI/Vulkan/VulkanTypes.h"

class VulkanShader final : public VkHwResource<vk::ShaderModule>, public RHIShader
{
public:
	VulkanShader(const class VulkanDevice& Device, const RHIShaderDesc& Desc);
};

class VulkanInputLayout final : public RHIInputLayout, public vk::PipelineVertexInputStateCreateInfo
{
public:
	VulkanInputLayout(const RHIInputLayoutDesc& Desc);

private:
	std::vector<vk::VertexInputBindingDescription> m_InputBindings;
	std::vector<vk::VertexInputAttributeDescription> m_InputAttributes;
};