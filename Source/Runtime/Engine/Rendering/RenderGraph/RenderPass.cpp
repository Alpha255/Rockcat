#include "Runtime/Engine/Rendering/RenderGraph/RenderPass.h"
#include "Runtime/Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Runtime/Engine/Scene/SceneView.h"

RenderPass::RenderPass(DAGNodeID ID, const char* Name, ResourceManager& ResourceMgr)
	: m_NodeID(ID)
	, m_Name(Name)
	, m_ResourceMgr(ResourceMgr)
{
	assert(Name);
}

Field& RenderPass::RegisterField(const char* Name, Field::EVisibility Visibility, Field::EResourceType Type)
{
    auto& Ret = m_ResourceMgr.GetOrAllocateField(Name, Visibility, Type);
    m_Fields.emplace_back(RenderPassField{Ret.GetNodeID(), Visibility});
    return Ret;
}

void RenderScene::GenerateDrawCommands()
{
	//std::unordered_map<const StaticMesh*, const MeshDrawCommand*> MeshCommandMap; // TODO: Do this when assimp scene loading?

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
				m_MeshPassDrawCommands[EMeshPass::PreDepth].push_back(&m_DrawCommands.back());
				m_MeshPassDrawCommands[EMeshPass::Opaque].emplace_back(&m_DrawCommands.back());
			}
			else if (Node->IsSkeletalMesh())
			{
				// TODO
			}
		}

		Node = SceneViewer.Next();
	}
}
