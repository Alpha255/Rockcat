#pragma once

#include "Runtime/Core/Math/Transform.h"
#include "Runtime/Engine/Asset/SceneAsset.h"
#include "Runtime/Engine/Scene/Components/StaticMesh.h"

class Scene : public SceneGraphAsset
{
public:
	using SceneGraphAsset::SceneGraphAsset;

	struct SceneData
	{
		std::vector<StaticMesh> StaticMeshes;
		std::vector<Math::Transform> Transforms;
	};

	template<class StringType>
	static std::shared_ptr<Scene> Load(StringType&& SceneGraphAssetPath) 
	{ 
		auto Ret = Cast<Scene>(SceneGraphAsset::Load(std::forward<StringType>(SceneGraphAssetPath)));
		Ret->LoadAssimpScenes();
		return Ret;
	}

	void Merge(const Scene& OtherScene);

	void Merge(const SceneGraph& OtherSceneGraph);

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

