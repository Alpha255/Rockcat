#include "Scene/Scene.h"
#include "Scene/SceneVisitor.h"
#include "Services/AssetDatabase.h"
#include "Components/TransformComponent.h"
#include "Components/StaticMeshComponent.h"

void Scene::Tick(float ElapsedSeconds)
{
	(void)ElapsedSeconds;
}

void Scene::MergeWithAssimpScene(const AssimpSceneAsset& AssimpScene)
{
	if (!HasEntity())
	{
		SetRoot(AssimpScene.Graph.GetRoot());

		auto& Entities = GetAllEntities();
		auto& AssimpSceneEntities = AssimpScene.Graph.GetAllEntities();
		Entities.insert(Entities.end(), AssimpSceneEntities.begin(), AssimpSceneEntities.end());

		for (auto& Entity : Entities)
		{
			auto It = AssimpScene.EntityDataIndices.find(Entity.GetID());
			if (It != AssimpScene.EntityDataIndices.end())
			{
				auto MeshIndex = It->second.Mesh;
				auto MaterialIndex = It->second.Material;
				auto TransformIndex = It->second.Transfrom;

				if (MeshIndex != ~0u)
				{
					auto StaticMeshComp = AddComponent<StaticMeshComponent>(Entity);
					StaticMeshComp->SetMesh(AssimpScene.StaticMeshes[MeshIndex]);

					if (MaterialIndex != ~0u)
					{
						StaticMeshComp->SetMaterial(AssimpScene.MaterialProperties[MaterialIndex]);
					}
				}
				if (TransformIndex != ~0u)
				{
					AddComponent<TransformComponent>(Entity)->SetTransform(AssimpScene.Transforms[TransformIndex]);
				}
			}
		}
	}
	else
	{

	}
}

void Scene::PostLoad()
{
	std::vector<const AssimpSceneAsset*> AssimpScenes;

	auto Callbacks = std::make_optional(Asset::AssetLoadCallbacks{});

	Callbacks.value().PreLoadCallback = [this, &AssimpScenes](Asset& InAsset) {
		AssimpScenes.push_back(Cast<AssimpSceneAsset>(&InAsset));
	};

	Callbacks.value().ReadyCallback = [this, &AssimpScenes](Asset&) {
		for (auto AssimpScene : AssimpScenes)
		{
			if (!AssimpScene->IsReady())
			{
				return;
			}
		}

		for (auto AssimpScene : AssimpScenes)
		{
			MergeWithAssimpScene(*AssimpScene);
		}

		BaseClass::PostLoad();
	};

	for (const auto& Path : m_AssimpScenes)
	{
		auto AssimpScenePath = Paths::GltfSampleModelPath() / Path;
		AssetDatabase::Get().GetOrReimportAsset<AssimpSceneAsset>(AssimpScenePath, Callbacks);
	}
}

Scene::~Scene()
{
	RemoveDyingEntities();
	Save(true);
}
