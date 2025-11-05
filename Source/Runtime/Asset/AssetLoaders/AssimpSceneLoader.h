#pragma once

#include "Asset/Asset.h"
#include "Scene/SceneGraph.h"

class AssimpSceneLoader : public AssetLoader
{
public:
	AssimpSceneLoader();

	bool Load(Asset& InAsset, const AssetType& Type) override final;
protected:
	std::shared_ptr<Asset> CreateAssetImpl(const std::filesystem::path& Path) override final;

	void ProcessTransform(const struct aiNode* AiNode, struct AssimpScene& Scene, class TransformComponent& TransformComp);
	bool ProcessNode(const struct aiScene* AiScene, const struct aiNode* AiNode, Entity& GraphNode, struct AssimpScene& Scene);
	void ProcessTextures(const struct aiMaterial* AiMaterial, struct MaterialProperty& Material, const std::filesystem::path& RootPath);

	void ProcessMaterial(const struct aiScene* AiScene, struct AssimpScene& Scene, uint32_t MaterialIndex, class StaticMeshComponent& StaticMeshComp);
	void ProcessMesh(const struct aiScene* AiScene, struct AssimpScene& Scene, uint32_t MeshIndex, class StaticMeshComponent& StaticMeshComp);
};