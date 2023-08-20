#include "Runtime/Engine/Asset/SceneAsset.h"
#include "Runtime/Engine/Scene/Scene.h"
#include "Runtime/Engine/Scene/SceneBuilder.h"
#include "Runtime/Engine/Engine.h"

void SceneGraphAsset::LoadAssimpScenes()
{
	SetStatus(EAssetStatus::Loading);

	std::vector<const AssimpSceneAsset*> AssimpScenes;

	auto AssetLoadCallbacks = std::make_optional(Asset::Callbacks{});
	AssetLoadCallbacks.value().PreLoadCallback = [this, &AssimpScenes](Asset& InAsset) {
		AssimpScenes.push_back(Cast<AssimpSceneAsset>(&InAsset));
	};
	AssetLoadCallbacks.value().ReadyCallback = [this, &AssimpScenes](Asset&) {
		this->RebuildSceneWhenReady(AssimpScenes);
	};

	for each (const auto& Path in m_AssimpScenePaths)
	{
		Engine::Get().GetAssetDatabase().FindOrLoadAsset<AssimpSceneAsset>(Path, AssetLoadCallbacks);
	}
}

void SceneGraphAsset::RebuildSceneWhenReady(std::vector<const AssimpSceneAsset*>& AssimpScenes)
{
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
		SceneBuilder::MergeSceneGraph(NewGraph, AssimpScene->GetSceneGraph());
	}

	Cast<Scene>(this)->Merge(NewGraph);

	SetStatus(EAssetStatus::Ready);
}
