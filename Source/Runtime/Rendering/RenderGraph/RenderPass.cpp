#include "Rendering/RenderGraph/RenderPass.h"
#include "Rendering/RenderGraph/RenderGraph.h"
#include "Rendering/RenderGraph/ResourceManager.h"
#include "RHI/RHIBackend.h"

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

const RenderSettings& RenderPass::GetRenderSettings() const
{
	return m_Graph.GetRenderSettings();
}

void RenderPass::AddField(RDGResourceID ID, RDGResource::EVisibility Visibility)
{
	for (auto& Field : m_Fields)
	{
		if (Field.ResourceID == ID)
		{
			Field.Visibility = Field.Visibility | Visibility;
			break;
		}
	}

	m_Fields.emplace_back(RenderPassField{ ID, Visibility });
}

std::vector<const RDGResource*> RenderPass::GetResources(RDGResource::EVisibility Visibility) const
{
	return std::vector<const RDGResource*>();
}

RDGResource& RenderPass::AddResource(RDGResource::EType Type, const char* Name, RDGResource::EVisibility Visibility)
{
	return GetResourceManager().GetOrAllocateResource(Type, Name, Visibility);
}
