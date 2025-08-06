#pragma once

#include "RHI/RHIPipeline.h"

class RHIPipelineManager
{
public:
	RHIPipelineManager(class RHIDevice& Device)
		: m_Device(Device)
	{
	}

	const RHIGraphicsPipeline* GetGraphicsPipeline(const RHIGraphicsPipelineDesc& Desc);
	const RHIComputePipeline* GetComputePipeline(const RHIComputePipelineDesc& Desc);
protected:
	template<class TDescription, class TPipeline>
	struct RHIPipelineProxy
	{
		TDescription Desc;
		std::shared_ptr<TPipeline> Pipeline;
		std::shared_ptr<TPipeline> FallbackPipeline;

		RHIPipelineProxy(const TDescription& InDesc)
			: Desc(InDesc)
		{
		}
	};

	using RHIGraphicsPipelineProxy = RHIPipelineProxy<RHIGraphicsPipelineDesc, RHIGraphicsPipeline>;
	using RHIComputePipelineProxy = RHIPipelineProxy<RHIComputePipelineDesc, RHIComputePipeline>;

	void RegisterShaderPipelineMap(std::string_view ShaderFile, RHIGraphicsPipelineProxy* Proxy);
private:
	std::unordered_map<size_t, RHIGraphicsPipelineProxy> m_GraphicsPipelineProxies;
	std::unordered_map<size_t, RHIComputePipelineProxy> m_ComputePipelineProxies;

	class RHIDevice& m_Device;
};