#include "Engine/Rendering/RenderGraph/RenderPass.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/Scene/SceneView.h"

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

RHIInterface& RenderPass::GetRHI()
{
	return m_ResourceMgr.GetRHI();
}

void RenderScene::GenerateDrawCommands()
{
	//std::unordered_map<const StaticMesh*, const MeshDrawCommand*> MeshCommandMap; // TODO: Do this when assimp scene loading?
	m_DrawCommands.clear();
	SceneView<BreadthFirst> SceneViewer(m_Scene);
	auto Node = SceneViewer.Get();
	while (std::_Get_unwrapped(Node))
	{
		if (Node->IsAlive() && Node->IsVisible())
		{
			if (Node->IsStaticMesh())
			{
				MeshDrawCommand NewCommand;
				NewCommand.Mesh = m_Scene.GetStaticMesh(Node->GetDataIndex());
				NewCommand.WorldTransform = m_Scene.GetTransform(Node->GetDataIndex());
				NewCommand.IsSelected = Node->IsSelected();

				m_DrawCommands.emplace_back(std::move(NewCommand));
				m_MeshDrawCommands[(size_t)EGeometryPassFilter::PreDepth].push_back(&m_DrawCommands.back());
				m_MeshDrawCommands[(size_t)EGeometryPassFilter::Opaque].push_back(&m_DrawCommands.back());
			}
			else if (Node->IsSkeletalMesh())
			{
				// TODO
			}
		}

		Node = SceneViewer.Next();
	}
}

const std::vector<std::shared_ptr<IView>>& RenderScene::GetViews() const
{
	if (m_Scene.IsDirty())
	{
		m_Views.clear();
	}

	return m_Views;
}