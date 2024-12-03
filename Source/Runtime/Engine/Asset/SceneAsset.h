#pragma once

#include "Core/Math/Transform.h"
#include "Engine/Asset/MaterialAsset.h"
#include "Engine/View/Camera.h"
#include "Engine/Scene/SceneGraph.h"
#include "Engine/Scene/Components/StaticMesh.h"

struct SceneData
{
	std::vector<std::shared_ptr<StaticMesh>> StaticMeshes;
	std::vector<std::shared_ptr<MaterialProperty>> MaterialProperties;
	std::vector<std::shared_ptr<Camera>> Cameras;
	std::vector<Math::Transform> Transforms;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Transforms),
			CEREAL_NVP(Cameras)
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
	const SceneGraph* GetGraph() const { return m_Graph.get(); }
	const SceneData* GetData() const { return m_Data.get(); }

	const StaticMesh* GetStaticMesh(uint32_t Index) const 
	{ 
		assert(Index < m_Data->StaticMeshes.size()); 
		return m_Data->StaticMeshes[Index].get(); 
	}

	const StaticMesh* GetStaticMesh(const SceneGraph::NodeID& ID) const
	{
		auto& Node = m_Graph->GetNode(ID);
		if (Node.IsStaticMesh())
		{
			return GetStaticMesh(Node.GetDataIndex());
		}
		return nullptr;
	}

	const Math::Transform* GetTransform(uint32_t Index) const
	{
		assert(Index < m_Data->StaticMeshes.size());
		return &m_Data->Transforms[Index];
	}

	const MaterialProperty* GetMaterialProperty(uint32_t Index) const
	{
		assert(Index < m_Data->MaterialProperties.size());
		return m_Data->MaterialProperties[Index].get();
	}

	const class Camera& GetCamera(uint32_t Index) const
	{
		assert(Index < m_Data->Cameras.size());
		return *m_Data->Cameras[Index];
	}

	const std::vector<std::shared_ptr<class Camera>>& GetCameras() const
	{
		return m_Data->Cameras;
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
