#pragma once

#include "Runtime/Engine/Asset/SerializableAsset.h"
#include "Runtime/Engine/Scene/Scene.h"

class SceneAsset : public SerializableAsset<SceneAsset>
{
public:
	ParentClass::ParentClass;

	const char8_t* GetExtension() const override final { return ".scene"; }

	const std::vector<std::string>& GetAssimpSceneAssetPath() const { return m_AssimpScenePaths; }
	std::shared_ptr<Scene::SceneGraph> GetSceneGraph() const { return m_SceneGraph; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_AssimpScenePaths),
			CEREAL_NVP(m_SceneGraph)
		);
	}
private:
	std::vector<std::string> m_AssimpScenePaths;
	std::shared_ptr<Scene::SceneGraph> m_SceneGraph;
};

class AssimpSceneAsset : public Asset
{
public:
	using Asset::Asset;
};
