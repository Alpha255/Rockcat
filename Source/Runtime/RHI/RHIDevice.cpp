#include "RHI/RHIDevice.h"

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

RHIFrameBuffer* RHIDevice::GetOrCreateFrameBuffer(const RHIFrameBufferCreateInfo& RHICreateInfo)
{
	size_t Hash = ComputeHash<RHIFrameBufferCreateInfo>(RHICreateInfo);

	auto It = m_FrameBufferCache.find(Hash);
	if (It == m_FrameBufferCache.end())
	{
		auto Result = m_FrameBufferCache.emplace(std::make_pair(Hash, CreateFrameBuffer(RHICreateInfo)));
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
