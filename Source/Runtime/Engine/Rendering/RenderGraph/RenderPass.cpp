#include "Engine/Rendering/RenderGraph/RenderPass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"

RenderPass::RenderPass(DAGNodeID ID, const char* Name, RenderGraph& Graph)
	: m_NodeID(ID)
	, m_Name(Name)
	, m_Graph(Graph)
{
	assert(Name);
}

ResourceManager& RenderPass::GetResourceManager()
{
	return m_Graph.GetResourceManager();
}

RDGResource& RenderPass::RegisterResource(const char* Name, RDGResource::EVisibility Visibility)
{
    auto& Ret = GetResourceManager().GetOrAllocateResource(Name, Visibility);
    m_Fields.emplace_back(RenderPassField{Ret.GetNodeID(), Visibility});
    return Ret;
}