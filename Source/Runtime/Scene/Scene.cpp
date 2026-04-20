#include "Scene/Scene.h"
#include "Scene/SceneVisitor.h"
#include "Services/AssetDatabase.h"
#include "Async/Task.h"
#include "Components/PrimitiveComponent.h"
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

void Scene::AddPrimitive(const PrimitiveComponent* PrimitiveComp)
{
	assert(TFTask::IsMainThread() || TFTask::IsGameThread());

	m_AddedPrimitives.push_back(PrimitiveComp);
}

void Scene::RemovePrimitive(const PrimitiveComponent* PrimitiveComp)
{
	assert(TFTask::IsMainThread() || TFTask::IsGameThread());

	m_RemovedPrimitives.push_back(PrimitiveComp);
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
				Integrate(*this, *Cast<AssimpScene>(LocalRequest.Target));
			}

			for (auto& Ent : GetAllEntities())
			{
				for (auto& PrimitiveComp : Ent.GetComponents<PrimitiveComponent>())
				{
					AddPrimitive(PrimitiveComp.get());
				}
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
