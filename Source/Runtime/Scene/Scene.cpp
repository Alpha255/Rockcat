#include "Scene/Scene.h"
#include "Scene/SceneVisitor.h"
#include "Services/AssetDatabase.h"
#include "Components/TransformComponent.h"
#include "Components/StaticMeshComponent.h"

void Scene::Tick(float ElapsedSeconds)
{
	(void)ElapsedSeconds;
}

void Scene::MergeWithAssimpScene(const AssimpScene& InScene)
{
	if (!HasEntity())
	{
		SetRoot(InScene.Graph.GetRoot());

		auto& Entities = GetAllEntities();
		auto& AssimpSceneEntities = InScene.Graph.GetAllEntities();
		Entities.insert(Entities.end(), AssimpSceneEntities.begin(), AssimpSceneEntities.end());

		for (auto& Entity : Entities)
		{
			auto It = InScene.EntityDataIndices.find(Entity.GetID());
			if (It != InScene.EntityDataIndices.end())
			{
				auto MeshIndex = It->second.Mesh;
				auto MaterialIndex = It->second.Material;
				auto TransformIndex = It->second.Transfrom;

				if (MeshIndex != ~0u)
				{
					auto StaticMeshComp = AddComponent<StaticMeshComponent>(Entity);
					StaticMeshComp->SetMesh(InScene.StaticMeshes[MeshIndex]);

					if (MaterialIndex != ~0u)
					{
						StaticMeshComp->SetMaterial(InScene.MaterialProperties[MaterialIndex]);
					}
				}
				if (TransformIndex != ~0u)
				{
					AddComponent<TransformComponent>(Entity)->SetTransform(InScene.Transforms[TransformIndex]);
				}
			}
		}
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
	RemoveDyingEntities();
	Save(true);
}
