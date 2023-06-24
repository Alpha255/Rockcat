#include "Runtime/Engine/Asset/SceneAsset.h"
#include "Runtime/Engine/Scene/Scene.h"
#include "Runtime/Engine/Scene/SceneBuilder.h"
#include "Runtime/Engine/Engine.h"

void SceneGraphAsset::LoadAssimpScenes()
{
	auto AssetLoadCallbacks = std::make_optional(Asset::Callbacks{});
	AssetLoadCallbacks.value().PreLoadCallback = [this](Asset& InAsset) {
		this->m_AssimpScenes.push_back(Cast<AssimpSceneAsset>(&InAsset));
	};
	AssetLoadCallbacks.value().ReadyCallback = [this](Asset&) {
		this->RebuildSceneWhenReady();
	};

	for each (const auto& Path in m_AssimpScenePaths)
	{
		Engine::Get().GetAssetDatabase().FindAsset<AssimpSceneAsset>(Path, AssetLoadCallbacks);
	}
}

void SceneGraphAsset::RebuildSceneWhenReady()
{
	for each (auto AssimpScene in m_AssimpScenes)
	{
		if (!AssimpScene->IsReady())
		{
			return;
		}
	}

	SetStatus(EAssetStatus::Loading);

	SceneBuilder Builder;

	SceneGraph NewGraph;
	for each (auto AssimpScene in m_AssimpScenes)
	{
		Builder.MergeSceneGraph(NewGraph, AssimpScene->GetSceneGraph());
	}

	Cast<Scene>(this)->Merge(NewGraph);

	SetStatus(EAssetStatus::Ready);
}
