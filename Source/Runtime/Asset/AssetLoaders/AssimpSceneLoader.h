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

	void ProcessTransform(const struct aiNode* AiNode, struct AssimpScene& Scene);
	bool ProcessNode(const struct aiScene* AiScene, const struct aiNode* AiNode, const EntityID GraphNode, struct AssimpScene& Scene);
	void ProcessMaterials(const struct aiScene* AiScene, struct AssimpScene& Scene);
	void ProcessMeshes(const struct aiScene* AiScene, struct AssimpScene& Asset);
	void ProcessTextures(const struct aiMaterial* AiMaterial, struct MaterialProperty& Material, const std::filesystem::path& RootPath);
};