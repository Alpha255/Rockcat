#include "Runtime/Engine/Asset/SceneAsset.h"
#include "Runtime/Engine/Scene/Scene.h"
#include "Runtime/Engine/Scene/SceneBuilder.h"
#include "Runtime/Engine/Engine.h"

void SceneAsset::PostLoad()
{
	std::vector<const AssimpScene*> AssimpScenes;

	auto AssetLoadCallbacks = std::make_optional(Asset::Callbacks{});

	AssetLoadCallbacks.value().PreLoadCallback = [this, &AssimpScenes](Asset& InAsset) {
		AssimpScenes.push_back(Cast<AssimpScene>(&InAsset));
	};

	AssetLoadCallbacks.value().ReadyCallback = [this, &AssimpScenes](Asset&) {
		for each (auto AssimpScene in AssimpScenes)
		{
			if (!AssimpScene->IsReady())
			{
				return;
			}
		}

		SceneGraph NewGraph;
		for each (auto AssimpScene in AssimpScenes)
		{
			SceneBuilder::MergeSceneGraph(NewGraph, AssimpScene->Graph);
		}
	};

	for each (const auto& Path in m_AssimpScenePaths)
	{
		AssetDatabase::Get().FindOrImportAsset<AssimpScene>(Path, AssetLoadCallbacks);
	}
}
