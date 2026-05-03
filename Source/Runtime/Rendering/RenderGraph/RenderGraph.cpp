#include "Rendering/RenderGraph/RenderGraph.h"
#include "Rendering/RenderGraph/ResourceManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneView.h"

RDGRenderGraph::RDGRenderGraph(const RenderSettings& Settings)
	: m_Settings(Settings)
{
}

void RDGRenderGraph::Execute()
{
}

RDGTexture* RDGRenderGraph::CreateTexture(const RHITextureDesc& Desc)
{
	return nullptr;
}

RDGBuffer* RDGRenderGraph::CreateBuffer(const RHIBufferDesc& Desc)
{
	return nullptr;
}

void RDGRenderGraph::Compile()
{
}
