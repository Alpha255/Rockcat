#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"
#include "Runtime/Engine/Rendering/RenderGraph/ResourceManager.h"

Field& RenderPass::RegisterField(
    const char8_t* Name,
    Field::EVisibility Visibility,
    Field::EResourceType Type,
    DAGNodeID RefID)
{
    auto& Ret = m_ResourceMgr.GetOrAllocateField(Name, Visibility, Type, RefID);
    m_Fields.emplace_back(RenderPassField{Ret.GetNodeID(), Visibility});
    return Ret;
}
