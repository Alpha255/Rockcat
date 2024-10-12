#pragma once

#include "Core/Math/Transform.h"
#include "Engine/Asset/MaterialAsset.h"
#include "Engine/Scene/SceneGraph.h"
#include "Engine/Scene/Components/StaticMesh.h"

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
	template<class T>
	SceneAsset(T&& Name)
		: BaseClass(GetFilePath(ASSET_PATH_SCENES, Name, GetExtension()))
	{
	}

	static const char* GetExtension() { return ".scene"; }
	const SceneGraph* GetSceneGraph() const { return m_Graph.get(); }
	const SceneData* GetSceneData() const { return m_Data.get(); }

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
			CEREAL_NVP(m_AssimpScenes),
			CEREAL_NVP(m_Graph),
			CEREAL_NVP(m_Data)
		);
	}
protected:
	friend class SceneBuilder;
	void PostLoad() override final;
private:
	std::vector<std::string> m_AssimpScenes;
	std::shared_ptr<SceneGraph> m_Graph;
	std::shared_ptr<SceneData> m_Data;
};

struct AssimpSceneAsset : public Asset
{
	using Asset::Asset;

	SceneGraph Graph;
	SceneData Data;
};
