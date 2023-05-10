#include "Colorful/IRenderer/RenderGraph/RenderGraphContext.h"
#include "Colorful/IRenderer/IRenderer.h"

NAMESPACE_START(RHI)

ISamplerSharedPtr RenderGraphContext::GetOrCreateSampler(const SamplerDesc& /*Desc*/)
{
    return ISamplerSharedPtr();
}

IPipelineSharedPtr RenderGraphContext::GetOrCreateGraphicsPipeline(const GraphicsPipelineDesc& /*Desc*/)
{
    return IPipelineSharedPtr();
}

NAMESPACE_END(RHI)