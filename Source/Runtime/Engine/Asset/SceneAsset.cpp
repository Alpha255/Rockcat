#include "Runtime/Engine/Asset/SceneAsset.h"
#include "Runtime/Engine/Scene/Scene.h"
#include "Runtime/Engine/Asset/AssetDatabase.h"

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
		if (Target.GetNodeCount() == 0u)
		{
			Target.Root = Other.Root;
			Target.IDAllocator = Other.IDAllocator;
			Target.Nodes.insert(Target.Nodes.end(), Other.Nodes.begin(), Other.Nodes.end());
		}
		else
		{
			// Traverse
		}
	}

	static void MergeSceneData(SceneData& Target, const SceneData& Other)
	{
		Target.StaticMeshes.insert(Target.StaticMeshes.end(), Other.StaticMeshes.begin(), Other.StaticMeshes.end());
		Target.Materials.insert(Target.Materials.end(), Other.Materials.begin(), Other.Materials.end());
		Target.Transforms.insert(Target.Transforms.end(), Other.Transforms.begin(), Other.Transforms.end());

		auto MeshIt = Target.StaticMeshes.end();
		MaterialID::IndexType StartIndex = static_cast<MaterialID::IndexType>(Target.Materials.size());
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
	std::vector<const AssimpScene*> AssimpScenes;

	auto AssetLoadCallbacks = std::make_optional(Asset::Callbacks{});

	AssetLoadCallbacks.value().PreLoadCallback = [this, &AssimpScenes](Asset& InAsset) {
		AssimpScenes.push_back(Cast<AssimpScene>(&InAsset));
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
		}

		// TODO: Thread safe??
		m_Graph.swap(NewGraph);
		m_Data.swap(NewData);
	};

	for (const auto& Path : m_AssimpScenePaths)
	{
		AssetDatabase::Get().FindOrImportAsset<AssimpScene>(Path, AssetLoadCallbacks);
	}
}
