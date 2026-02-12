#include "Scene/Scene.h"
#include "Scene/SceneVisitor.h"
#include "Services/AssetDatabase.h"
#include "Components/TransformComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Paths.h"

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

void Scene::MergeWithAssimpScene(const AssimpScene& AiScene)
{
}

void Scene::OnPostLoad()
{
	m_AssimpLoadRequests.reset(new AssetLoadRequests());
	m_AssimpLoadRequests->reserve(m_AssimpScenes.size());

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

		m_AssimpLoadRequests->emplace_back(AssetLoadRequest{AiScenePath.string()});
	}

	for (auto& Request : *m_AssimpLoadRequests)
	{
		Request.OnLoaded = [LocalLoadRequests = m_AssimpLoadRequests.get(), this](Asset&) {
			for (const auto& LocalRequest : *LocalLoadRequests)
			{
				if (!LocalRequest.Target->IsReady(std::memory_order_acquire))
				{
					return;
				}
			}

			for (const auto& LocalRequest : *LocalLoadRequests)
			{
				MergeWithAssimpScene(*Cast<AssimpScene>(LocalRequest.Target));
			}

			SetStatus(EStatus::Ready);
		};
	}

	AssetDatabase::Get().RequestLoad(*m_AssimpLoadRequests);
}

Scene::~Scene()
{
	RemoveInvalidEntities();
	Save(true);
}
