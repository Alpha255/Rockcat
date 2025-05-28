#include "Asset/SceneAsset.h"
#include "Scene/Scene.h"
#include "Services/AssetDatabase.h"

class SceneBuilder
{
public:
	static void MergeScene(Scene& Target, const Scene& Other)
	{
		MergeSceneGraph(*Target.m_Graph, *Other.m_Graph);
		MergeSceneData(*Target.m_Data, *Other.m_Data);
	}

	static void MergeSceneGraph(SceneGraph& Target, const SceneGraph& Other)
	{
		if (Target.Nodes.empty())
		{
			Target.Root = Other.Root;
			Target.Nodes.insert(Target.Nodes.end(), Other.Nodes.begin(), Other.Nodes.end());
		}
		else
		{
			auto const StartIndex = static_cast<EntityID::IndexType>(Target.Nodes.size());
			for (auto& Node : Other.Nodes)
			{
				auto GraphNodeID = Target.AddNode(EntityID(), Node.GetName(), Node.GetMasks());
				auto GraphNode = const_cast<SceneGraph::Node*>(Target.GetNode(GraphNodeID));
				auto Parent = Node.HasParent() ? Node.GetParent() + StartIndex : Target.Root;
				auto Child = Node.HasChild() ? Node.GetChild() + StartIndex : EntityID();
				auto Sibling = Node.HasSibling() ? Node.GetSibling() + StartIndex : EntityID();
				
				GraphNode->SetVisible(Node.IsVisible())
					.SetAlive(Node.IsAlive())
					.SetParent(Parent)
					.SetChild(Child)
					.SetSibling(Sibling);
			}
		}
	}

	static void MergeSceneData(SceneData& Target, const SceneData& Other)
	{
		Target.StaticMeshes.insert(Target.StaticMeshes.end(), Other.StaticMeshes.begin(), Other.StaticMeshes.end());
		Target.MaterialProperties.insert(Target.MaterialProperties.end(), Other.MaterialProperties.begin(), Other.MaterialProperties.end());
		Target.Transforms.insert(Target.Transforms.end(), Other.Transforms.begin(), Other.Transforms.end());

		auto MeshIt = Target.StaticMeshes.end();
		MaterialID StartIndex = static_cast<MaterialID>(Target.MaterialProperties.size());
		if (StartIndex > 0u)
		{
			for (; MeshIt < Target.StaticMeshes.end(); ++MeshIt)
			{
				(*MeshIt)->SetMaterialID((*MeshIt)->GetMaterialID() + StartIndex);
			}
		}
	}
};
