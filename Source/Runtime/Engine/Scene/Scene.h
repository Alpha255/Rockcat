#pragma once

#include "Runtime/Engine/Asset/SceneAsset.h"

class Scene : public SceneGraphAsset
{
public:
	using SceneGraphAsset::SceneGraphAsset;

	static Scene* Load(const char8_t* SceneGraphAssetPath) { return Cast<Scene>(SceneGraphAsset::Load(SceneGraphAssetPath).get()); }

	void Merge(const Scene& OtherScene);

	struct SceneData
	{
	};

	const SceneData& GetSceneData() const { return m_Data; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneGraphAsset);
		)
	}
protected:
	friend class SceneBuilder;
private:
	SceneData m_Data;
};

