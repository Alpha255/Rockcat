#pragma once

#include "Runtime/Asset/IImporter.h"
#include "Runtime/Asset/Model.h"

struct aiScene;
struct aiNode;
struct aiMesh;

class AssimpImporter : public IImporter
{
public:
	using IImporter::IImporter;

	void Reimport(std::shared_ptr<IAsset> Asset) override final;
protected:
	void ProcessNode(const aiScene* AssimpScene, const aiNode* AssimpNode, ModelAsset* Model);
	std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Material>> ProcessMesh(const aiScene* AssimpScene, const aiMesh* AssimpMesh, ModelAsset* Model);
	std::shared_ptr<Material> ProcessMaterial(const aiScene* AssimpScene, const aiMesh* AssimpMesh, ModelAsset* Model);
};
