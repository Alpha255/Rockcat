#pragma once

#include "Core/Math/Transform.h"
#include "Asset/Material.h"
#include "Scene/SceneGraph.h"
#include "Scene/Components/Camera.h"
#include "Scene/Components/StaticMesh.h"
#include "Paths.h"

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
	SceneAsset(std::filesystem::path&& Name)
		: BaseClass(Paths::ScenePath() / Name)
	{
	}

	static const char* GetExtension() { return ".scene"; }
	const SceneGraph& GetGraph() const { assert(m_Graph); return *m_Graph; }
	const SceneData& GetData() const { assert(m_Data); return *m_Data; }

	const StaticMesh* GetStaticMesh(uint32_t Index) const 
	{ 
		assert(Index < m_Data->StaticMeshes.size()); 
		return m_Data->StaticMeshes[Index].get(); 
	}

	const StaticMesh* GetStaticMesh(const SceneGraph::NodeID& ID) const
	{
		if (auto Node = m_Graph->GetNode(ID))
		{
			if (Node->IsPrimitive())
			{
				return GetStaticMesh(Node->GetDataIndex());
			}
		}
		return nullptr;
	}

	const Math::Transform& GetTransform(uint32_t Index) const
	{
		assert(Index < m_Data->StaticMeshes.size());
		return m_Data->Transforms[Index];
	}

	const MaterialProperty& GetMaterialProperty(uint32_t Index) const
	{
		assert(Index < m_Data->MaterialProperties.size());
		return *m_Data->MaterialProperties[Index];
	}

	Camera& GetCamera(uint32_t Index) const
	{
		assert(Index < m_Data->Cameras.size());
		return *m_Data->Cameras[Index];
	}

	Camera& GetMainCamera() const
	{
		assert(m_Data->Cameras.size() > 0u);
		return *m_Data->Cameras[0];
	}

	std::vector<std::shared_ptr<Camera>>& GetCameras() const
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
	SceneGraph& GetGraph() { assert(m_Graph); return *m_Graph; }
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
