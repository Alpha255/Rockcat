#pragma once

#include "Runtime/Engine/Asset/SerializableAsset.h"
#include "Runtime/Engine/Scene/Scene.h"

class SceneAsset : public SerializableAsset<SceneAsset>
{
public:
	ParentClass::ParentClass;

	const char8_t* GetAssetExtension() const override final { return ".scene"; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_SceneAssetPaths),
			CEREAL_NVP(m_SceneGraph)
		);
	}
private:
	std::vector<std::string> m_SceneAssetPaths;
	std::shared_ptr<Scene::SceneGraph> m_SceneGraph;
};

