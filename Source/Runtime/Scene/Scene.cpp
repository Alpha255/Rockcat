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

void Scene::MergeAssimpScenes(std::vector<std::shared_ptr<AssimpScene>>* AssimpScenes)
{
	if (AssimpScenes->size() != m_AssimpScenePaths.size())
	{
		return;
	}

	for (auto& AssimpScene : *AssimpScenes)
	{
		if (!AssimpScene)
		{
			continue;
		}

		if (!AssimpScene->IsReady())
		{
			return;
		}
	}

	for (auto& AssimpScene : *AssimpScenes)
	{
		if (!AssimpScene)
		{
			continue;
		}
	}

	SetStatus(EStatus::Ready);

	delete AssimpScenes;
}

void Scene::SetStatusChangeCallbacks()
{
	BaseClass::SetStatusChangeCallbacks();

	std::vector<std::shared_ptr<AssimpScene>>* AssimpScenes = new std::vector<std::shared_ptr<AssimpScene>>();

	SetOnPostLoad([this, AssimpScenes]() {
		for (const auto& Path : m_AssimpScenePaths)
		{
			std::filesystem::path AssimpScenePath = Path;
			if (!std::filesystem::exists(AssimpScenePath))
			{
				AssimpScenePath = Paths::GltfSampleModelPath() / Path;
				if (!std::filesystem::exists(AssimpScenePath))
				{
					LOG_WARNING("Assimp asset \"{}\" not exists", Path);
					continue;
				}
			}

			/// @TODO: Thread safe ???
			auto AssimpSceneOnLoading = AssimpScenes->emplace_back(AssetDatabase::Get().Load<AssimpScene>(AssimpScenePath)).get();
			AssimpSceneOnLoading->SetOnReady([this, AssimpScenes, AssimpSceneOnLoading]() {
				AssimpSceneOnLoading->SetStatus(EStatus::Ready);
				MergeAssimpScenes(AssimpScenes);
			});
		}
	});
}

Scene::~Scene()
{
	RemoveInvalidEntities();
	Save(true);
}
