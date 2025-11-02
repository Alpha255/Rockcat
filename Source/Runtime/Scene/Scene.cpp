#include "Scene/Scene.h"
#include "Scene/SceneVisitor.h"
#include "Services/AssetDatabase.h"
#include "Components/TransformComponent.h"
#include "Components/StaticMeshComponent.h"

void Scene::Tick(float ElapsedSeconds)
{
	if (!IsReady())
	{
		return;
	}

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

void Scene::MergeAssimpScenes()
{
	for (auto& AssimpScene : m_AssimpScenes)
	{
		if (!AssimpScene->IsReady())
		{
			return;
		}
	}

	Asset::OnPostLoad();
}

void Scene::OnPostLoad()
{
	for (const auto& Path : m_AssimpScenePaths)
	{
		if (std::filesystem::exists(Path))
		{
			m_AssimpScenes.emplace_back(AssetDatabase::Get().Load<AssimpScene>(Path));
		}
		else
		{
			auto AssimpScenePath = Paths::GltfSampleModelPath() / Path;
			if (std::filesystem::exists(AssimpScenePath))
			{
				m_AssimpScenes.emplace_back(AssetDatabase::Get().Load<AssimpScene>(Path));
			}
			else
			{
				LOG_ERROR("The assimp scene asset \"{}\" is not exists", Path);
			}
		}
	}

	// std::vector<const AssimpScene*> AssimpScenes;
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
