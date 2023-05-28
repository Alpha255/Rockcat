#if 0
#include "Colorful/IRenderer/IDevice.h"

NAMESPACE_START(RHI)

ISamplerSharedPtr IDevice::GetOrCreateSampler(const SamplerDesc& Desc)
{
	auto Hash = Desc.Hash();

	std::lock_guard ScopedLocker(m_Samplers.first);

	auto& Sampler = m_Samplers.second[Hash];
	if (!Sampler)
	{
		Sampler = CreateSampler(Desc);
	}

	return Sampler;
}

IInputLayoutSharedPtr IDevice::GetOrCreateInputLayout(const InputLayoutDesc& Desc, const ShaderDesc& VertexShaderDesc)
{
	auto Hash = Desc.Hash();

	std::lock_guard ScopedLocker(m_InputLayouts.first);

	auto& InputLayout = m_InputLayouts.second[Hash];
	if (!InputLayout)
	{
		InputLayout = CreateInputLayout(Desc, VertexShaderDesc);
	}

	return InputLayout;
}

IPipelineSharedPtr IDevice::GetOrCreateGraphicsPipeline(const GraphicsPipelineDesc& Desc)
{
	auto& Ret = m_GraphicsPipelineCache[Desc.Hash()];
	if (!Ret)
	{
		Ret = CreateGraphicsPipeline(Desc);
	}

	return Ret;
}

IFrameBufferSharedPtr IDevice::GetOrCreateFrameBuffer(const FrameBufferDesc& Desc)
{
	auto& Ret = m_FrameBufferCache[Desc.Hash()];
	if (!Ret)
	{
		Ret = CreateFrameBuffer(Desc);
	}

	return Ret;
}

NAMESPACE_END(RHI)
#endif