#pragma once

#include "Runtime/Core/Math/Transform.h"
#include "Runtime/Engine/Asset/MaterialAsset.h"
#include "Runtime/Engine/Scene/SceneGraph.h"
#include "Runtime/Engine/Scene/Components/StaticMesh.h"

struct SceneData
{
	std::vector<std::shared_ptr<StaticMesh>> StaticMeshes;
	std::vector<std::shared_ptr<MaterialAsset>> Materials;
	std::vector<Math::Transform> Transforms;
};

class SceneAsset : public SerializableAsset<SceneAsset>
{
public:
	template<class StringType>
	SceneAsset(StringType&& SceneAssetName)
		: BaseClass(Asset::GetPrefabricateAssetPath(SceneAssetName, Asset::EPrefabAssetType::Scene))
	{
	}

	const char8_t* GetExtension() const override final { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::Scene); }

	std::shared_ptr<SceneGraph> GetSceneGraph() const { return m_Graph; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_AssimpScenePaths),
			CEREAL_NVP(m_Graph)
		);
	}
protected:
	void PostLoad() override final;
private:
	std::vector<std::string> m_AssimpScenePaths;
	std::shared_ptr<SceneGraph> m_Graph;
};

struct AssimpScene : public Asset
{
	using Asset::Asset;

	SceneGraph Graph;
	SceneData Data;
};
