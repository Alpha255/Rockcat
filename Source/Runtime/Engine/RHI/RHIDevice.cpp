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

#include "Engine/RHI/RHIDevice.h"

RHIGraphicsPipeline* RHIDevice::GetOrCreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& RHICreateInfo)
{
	size_t Hash = ComputeHash<RHIGraphicsPipelineCreateInfo>(RHICreateInfo);

	auto It = m_GraphicsPipelineCache.find(Hash);
	if (It == m_GraphicsPipelineCache.end())
	{
		auto Result = m_GraphicsPipelineCache.emplace(std::make_pair(Hash, CreateGraphicsPipeline(RHICreateInfo)));
		if (Result.second)
		{
			return Result.first->second.get();
		}
		else
		{
			return nullptr;
		}
	}
	else
	{
		return It->second.get();
	}
}
