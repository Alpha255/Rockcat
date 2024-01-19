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

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Transforms)
		);
	}
};

class SceneAsset : public SerializableAsset<SceneAsset>
{
public:
	template<class StringType>
	SceneAsset(StringType&& SceneAssetName)
		: BaseClass(Asset::GetPrefabricateAssetPath(SceneAssetName, Asset::EPrefabAssetType::Scene))
	{
	}

	const char* GetExtension() const override final { return Asset::GetPrefabricateAssetExtension(Asset::EPrefabAssetType::Scene); }

	const SceneGraph& GetSceneGraph() const { return *m_Graph; }

	const SceneData& GetSceneData() const { return *m_Data; }

	const StaticMesh* GetStaticMesh(uint32_t Index) const
	{
		assert(Index < m_Data->StaticMeshes.size());
		return m_Data->StaticMeshes[Index].get();
	}

	const Math::Transform* GetTransform(uint32_t Index) const
	{
		assert(Index < m_Data->StaticMeshes.size());
		return &m_Data->Transforms[Index];
	}

	const MaterialAsset* GetMaterial(uint32_t Index) const
	{
		assert(Index < m_Data->Materials.size());
		return m_Data->Materials[Index].get();
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_AssimpScenePaths),
			CEREAL_NVP(m_Graph),
			CEREAL_NVP(m_Data)
		);
	}
protected:
	friend class SceneBuilder;
	void PostLoad() override final;
private:
	std::vector<std::string> m_AssimpScenePaths;
	std::shared_ptr<SceneGraph> m_Graph;
	std::shared_ptr<SceneData> m_Data;
};

struct AssimpScene : public Asset
{
	using Asset::Asset;

	SceneGraph Graph;
	SceneData Data;
};
