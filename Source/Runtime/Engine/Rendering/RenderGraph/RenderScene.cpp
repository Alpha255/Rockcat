#include "Engine/Rendering/RenderGraph/RenderScene.h"
#include "Engine/Scene/SceneViewer.h"

void RenderScene::GenerateDrawCommands()
{
	//std::unordered_map<const StaticMesh*, const MeshDrawCommand*> MeshCommandMap; // TODO: Do this when assimp scene loading?
	m_DrawCommands.clear();

	SceneViewer<BreadthFirst> SceneViewer(m_Scene);
	auto Node = SceneViewer.Get();

	while (std::_Get_unwrapped(Node))
	{
		if (Node->IsAlive() && Node->IsVisible())
		{
			if (Node->IsStaticMesh())
			{
				MeshDrawCommand NewCommand;

				m_DrawCommands.emplace_back(std::move(NewCommand));
				m_MeshDrawCommands[(size_t)EGeometryPassFilter::PreDepth].push_back(&m_DrawCommands.back());
				m_MeshDrawCommands[(size_t)EGeometryPassFilter::Opaque].push_back(&m_DrawCommands.back());
			}
			else if (Node->IsSkeletalMesh())
			{
				assert(false);
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