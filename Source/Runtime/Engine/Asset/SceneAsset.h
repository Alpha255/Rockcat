#pragma once

#include "Runtime/Core/Math/Transform.h"
#include "Runtime/Engine/Asset/SerializableAsset.h"
#include "Runtime/Engine/Scene/SceneGraph.h"
#include "Runtime/Engine/Scene/Components/StaticMesh.h"

struct SceneData
{
	std::vector<StaticMesh> StaticMeshes;
	std::vector<Math::Transform> Transforms;
};

class SceneAsset : public SerializableAsset<SceneAsset>
{
public:
	template<class StringType>
	SceneAsset(StringType&& SceneAssetName)
		: ParentClass(Asset::GetPrefabricateAssetPath(SceneAssetName, Asset::EPrefabAssetType::Scene))
		, m_Graph(std::move(std::make_shared<SceneGraph>()))
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

	void PostLoad() override final;
protected:
private:
	std::vector<std::string> m_AssimpScenePaths;
	std::shared_ptr<SceneGraph> m_Graph;
};

class AssimpSceneAsset : public Asset
{
public:
	using Asset::Asset;

	const SceneGraph& GetSceneGraph() const { return m_SceneGraph; }
	SceneGraph& GetSceneGraph() { return m_SceneGraph; }

	const SceneData& GetSceneData() const { return m_SceneData; }
private:
	SceneGraph m_SceneGraph;
	SceneData m_SceneData;
};
