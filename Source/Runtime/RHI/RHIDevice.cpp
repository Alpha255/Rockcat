#include "RHI/RHIDevice.h"

RHIGraphicsPipeline* RHIDevice::GetOrCreateGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc)
{
	size_t Hash = ComputeHash<RHIGraphicsPipelineDesc>(Desc);

	auto It = m_GraphicsPipelineCache.find(Hash);
	if (It == m_GraphicsPipelineCache.end())
	{
		auto Result = m_GraphicsPipelineCache.emplace(std::make_pair(Hash, CreateGraphicsPipeline(Desc)));
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

RHIFrameBuffer* RHIDevice::GetOrCreateFrameBuffer(const RHIFrameBufferDesc& Desc)
{
	size_t Hash = ComputeHash<RHIFrameBufferDesc>(Desc);

	auto It = m_FrameBufferCache.find(Hash);
	if (It == m_FrameBufferCache.end())
	{
		auto Result = m_FrameBufferCache.emplace(std::make_pair(Hash, CreateFrameBuffer(Desc)));
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
