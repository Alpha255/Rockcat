#pragma once

#include "Asset/Asset.h"

class AssimpSceneLoader : public AssetLoader
{
public:
	AssimpSceneLoader();

	bool Load(Asset& Target) override final;
protected:
	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& Path) override final;

	void ProcessTransform(const struct aiNode* AiNode, class TransformComponent& TransformComp);
	bool ProcessNode(const struct aiScene* AiScene, const struct aiNode* AiNode, const class Entity* Parent, struct AssimpScene& Model);
	void ProcessTextures(const struct aiMaterial* AiMaterial, struct MaterialProperty& Material, const std::filesystem::path& RootPath);

	void ProcessMaterial(const struct aiScene* AiScene, const struct AssimpScene& Model, uint32_t MaterialIndex, class StaticMeshComponent& StaticMeshComp);
	void ProcessMesh(const struct aiScene* AiScene, uint32_t MeshIndex, class StaticMeshComponent& StaticMeshComp);
};