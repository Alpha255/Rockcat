#include "Colorful/IRenderer/RenderGraph/RenderGraphContext.h"
#include "Colorful/IRenderer/IRenderer.h"

NAMESPACE_START(RHI)

ISamplerPtr RenderGraphContext::GetOrCreateSampler(const SamplerDesc& /*Desc*/)
{
    return ISamplerPtr();
}

IPipelinePtr RenderGraphContext::GetOrCreateGraphicsPipeline(const GraphicsPipelineDesc& /*Desc*/)
{
    return IPipelinePtr();
}

NAMESPACE_END(RHI)