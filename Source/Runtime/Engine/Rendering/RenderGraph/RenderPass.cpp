#include "Engine/Rendering/RenderGraph/RenderPass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/RHI/RHIInterface.h"

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

const GraphicsSettings& RenderPass::GetGraphicsSettings() const
{
	return m_Graph.GetBackend().GetGraphicsSettings();
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

RDGResource& RenderPass::AddResource(RDGResource::EType Type, const char* Name, RDGResource::EVisibility Visibility)
{
	return GetResourceManager().GetOrAllocateResource(Type, Name, Visibility);
}
