#include "Scene/Scene.h"
#include "Scene/SceneVisitor.h"
#include "Services/AssetDatabase.h"
#include "Components/TransformComponent.h"
#include "Components/StaticMeshComponent.h"

void Scene::Tick(float ElapsedSeconds)
{
	for (auto& Entity : GetAllEntities())
	{
		if (!Entity.IsAlive() || !Entity.IsVisible())
		{
			continue;
		}

		for (auto Comp : Entity.GetAllComponents())
		{
			if (Comp)
			{
				Comp->Tick(ElapsedSeconds);
			}
		}
	}
}

void Scene::Merge(const AssimpScene& InScene)
{
	if (!HasEntity())
	{
		SetRoot(InScene.Graph.GetRoot());
	}
	else
	{

	}
}

void Scene::OnPostLoad()
{
	//std::vector<const AssimpScene*> AssimpScenes;

	//auto Callbacks = std::make_optional(Asset::AssetLoadCallbacks{});

	//Callbacks.value().PreLoadCallback = [this, &AssimpScenes](Asset& InAsset) {
	//	AssimpScenes.push_back(Cast<AssimpSceneAsset>(&InAsset));
	//};

	//Callbacks.value().ReadyCallback = [this, &AssimpScenes](Asset&) {
	//	for (auto AssimpScene : AssimpScenes)
	//	{
	//		if (!AssimpScene->IsReady())
	//		{
	//			return;
	//		}
	//	}

	//	for (auto AssimpScene : AssimpScenes)
	//	{
	//		MergeWithAssimpScene(*AssimpScene);
	//	}

	//	BaseClass::OnPostLoad();
	//};

	//for (const auto& Path : m_AssimpScenes)
	//{
	//	auto AssimpScenePath = Paths::GltfSampleModelPath() / Path;
	//	AssetDatabase::Get().GetOrReimportAsset<AssimpScene>(AssimpScenePath, Callbacks);
	//}
}

Scene::~Scene()
{
	RemoveInvalidEntities();
	Save(true);
}
