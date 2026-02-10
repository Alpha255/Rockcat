#include "Scene/Scene.h"
#include "Scene/SceneVisitor.h"
#include "Services/AssetDatabase.h"
#include "Components/TransformComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Paths.h"

void AssimpScene::OnPostLoad()
{

}

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

void Scene::MergeWithAssimpScenes(const AssimpScene& AiScene)
{
	//if (AssimpScenes->size() != m_AssimpScenes.size())
	//{
	//	return;
	//}

	//for (auto& AssimpScene : *AssimpScenes)
	//{
	//	if (!AssimpScene)
	//	{
	//		continue;
	//	}

	//	if (!AssimpScene->IsReady(std::memory_order_acquire))
	//	{
	//		return;
	//	}
	//}

	//for (auto& AssimpScene : *AssimpScenes)
	//{
	//	if (!AssimpScene)
	//	{
	//		continue;
	//	}
	//}

	//SetStatus(EStatus::Ready);

	//delete AssimpScenes;
}

void Scene::OnPostLoad()
{
	for (const auto& Path : m_AssimpScenes)
	{
		std::filesystem::path AiScenePath(Path);
		if (!std::filesystem::exists(AiScenePath))
		{
			AiScenePath = Paths::GltfSampleModelPath() / Path;
			if (!std::filesystem::exists(AiScenePath))
			{
				LOG_WARNING("Assimp scene \"{}\" not exists", Path);
				continue;
			}
		}
	}

	//	/// @TODO: Thread safe ???
	//	auto AssimpSceneOnLoading = AssimpScenes->emplace_back(AssetDatabase::Get().Load<AssimpScene>(AssimpScenePath));
	//	AssimpSceneOnLoading->MergeAssimpScenes(AssimpScenes);
	//}
}

Scene::~Scene()
{
	RemoveInvalidEntities();
	Save(true);
}
