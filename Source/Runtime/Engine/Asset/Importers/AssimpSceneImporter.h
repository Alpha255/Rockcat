#pragma once

#include "Runtime/Engine/Asset/SceneAsset.h"

class AssimpSceneImporter : public IAssetImporter
{
public:
	AssimpSceneImporter()
		: IAssetImporter({".3ds", ".blend", ".dae", ".fbx", ".gltf", ".mdl", ".obj", ".ply", ".stl", ".x", ".raw"})
	{
	}

	std::shared_ptr<Asset> CreateAsset(const std::filesystem::path& AssetPath) override final { return std::make_shared<AssimpSceneAsset>(AssetPath); }

	void Reimport(Asset& InAsset) override final
	{
	}
protected:
	//void ProcessNode(const aiScene* AssimpScene, const aiNode* AssimpNode, ModelAsset* Model);
	//std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>> ProcessMesh(const aiScene* AssimpScene, const aiMesh* AssimpMesh, ModelAsset* Model);
	//std::shared_ptr<Material> ProcessMaterial(const aiScene* AssimpScene, const aiMesh* AssimpMesh, ModelAsset* Model);
};
