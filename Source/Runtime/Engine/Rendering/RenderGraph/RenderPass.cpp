#include "Engine/Rendering/RenderGraph/RenderPass.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"

RenderPass::RenderPass(DAGNodeID ID, const char* Name, ResourceManager& ResourceMgr)
	: m_NodeID(ID)
	, m_Name(Name)
	, m_ResourceMgr(ResourceMgr)
{
	assert(Name);
}

RDGResource& RenderPass::RegisterResource(const char* Name, RDGResource::EVisibility Visibility)
{
    auto& Ret = m_ResourceMgr.GetOrAllocateResource(Name, Visibility);
    m_Fields.emplace_back(RenderPassField{Ret.GetNodeID(), Visibility});
    return Ret;
}