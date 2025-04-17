#include "Engine/Asset/SceneAsset.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/AssetDatabase.h"

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
			auto const StartIndex = static_cast<SceneGraph::NodeID::IndexType>(Target.Nodes.size());
			for (auto& Node : Other.Nodes)
			{
				auto GraphNodeID = Target.AddNode(SceneGraph::NodeID(), Node.GetName(), Node.GetMasks());
				auto GraphNode = const_cast<SceneGraph::Node*>(Target.GetNode(GraphNodeID));
				auto Parent = Node.HasParent() ? Node.GetParent() + StartIndex : Target.Root;
				auto Child = Node.HasChild() ? Node.GetChild() + StartIndex : SceneGraph::NodeID();
				auto Sibling = Node.HasSibling() ? Node.GetSibling() + StartIndex : SceneGraph::NodeID();
				
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

void SceneAsset::PostLoad()
{
	std::vector<const AssimpSceneAsset*> AssimpScenes;

	auto AssetLoadCallbacks = std::make_optional(Asset::Callbacks{});

	AssetLoadCallbacks.value().PreLoadCallback = [this, &AssimpScenes](Asset& InAsset) {
		AssimpScenes.push_back(Cast<AssimpSceneAsset>(&InAsset));
	};

	AssetLoadCallbacks.value().ReadyCallback = [this, &AssimpScenes](Asset&) {
		for (auto AssimpScene : AssimpScenes)
		{
			if (!AssimpScene->IsReady())
			{
				return;
			}
		}

		auto NewGraph = std::make_shared<SceneGraph>();
		auto NewData = std::make_shared<SceneData>();
		for (auto AssimpScene : AssimpScenes)
		{
			SceneBuilder::MergeSceneGraph(*NewGraph, AssimpScene->Graph);
			SceneBuilder::MergeSceneData(*NewData, AssimpScene->Data);

			NewGraph->NumPrimitives += AssimpScene->Graph.NumPrimitives;
		}

		if (NewData->Transforms.empty())
		{
			NewData->Transforms.resize(NewGraph->Nodes.size());
		}

		// TODO: Thread safe??
		m_Graph.swap(NewGraph);
		m_Data.swap(NewData);
	};

	for (const auto& Path : m_AssimpScenes)
	{
		auto SceneAssetPath = Paths::GltfSampleModelPath() / Path;
		AssetDatabase::Get().GetOrReimportAsset<AssimpSceneAsset>(SceneAssetPath, AssetLoadCallbacks);
	}

	if (m_Data->Cameras.empty())
	{
		GetGraph().AddNode(SceneGraph::NodeID(), "MainCamera", SceneGraph::Node::ENodeMasks::Camera);
		m_Data->Cameras.emplace_back(std::make_shared<Camera>(Camera::EMode::FirstPerson));
	}
}
