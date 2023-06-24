#pragma once

#include "Runtime/Engine/Asset/SerializableAsset.h"
#include "Runtime/Engine/Scene/SceneGraph.h"

class SceneGraphAsset : public SerializableAsset<SceneGraphAsset>
{
public:
	template<class StringType>
	SceneGraphAsset(StringType&& SceneGraphAssetName)
		: ParentClass(Asset::GetPrefabricateAssetPath(SceneGraphAssetName, Asset::EPrefabricateAssetType::SceneAsset))
	{
	}

	const char8_t* GetExtension() const override final { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabricateAssetType::SceneAsset); }

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
	void LoadAssimpScenes();
private:
	std::vector<std::string> m_AssimpScenePaths;
	std::vector<const class AssimpSceneAsset*> m_AssimpScenes;
	std::shared_ptr<SceneGraph> m_Graph;
};

class AssimpSceneAsset : public Asset
{
public:
	using Asset::Asset;
};
