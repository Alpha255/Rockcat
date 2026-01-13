#pragma once

#include "Rendering/RenderGraph/RenderPassField.h"
#include "RHI/RHIRenderStates.h"
#include "RHI/RHIPipeline.h"
#include "Asset/Material.h"
#include "Scene/SceneGraph.h"
#include "Rendering/SceneView.h"
#include "Rendering/MeshDrawCommand.h"

class RenderScene
{
public:
	RenderScene(const class Scene& InScene, bool AsyncMeshDrawCommandsBuilding);

	const class Scene& GetScene() const { return m_Scene; }
	const std::vector<std::shared_ptr<MeshDrawCommand>>& GetCommands(EGeometryPass Filter) const { return m_Commands[Filter]; }

	void BuildMeshDrawCommands(const class ISceneView& SceneView);
	void WaitCommandsBuilding();

	static void RegisterMeshDrawCommandBuilder(EGeometryPass Filter, struct MeshDrawCommandBuilder* Builder);
protected:
	template<class GeometryPass>
	inline struct MeshDrawCommandBuilder* GetBuilder(GeometryPass Filter) { return s_Builders[Filter].get(); }
private:
	void GetScenePrimitives();
	void UpdateScenePrimitives();
	void RemoveInvalidCommands();

	void UpdateMeshBatch(uint32_t MeshIndex, int32_t Add);

	const class Scene& m_Scene;

	struct PrimitiveCollection
	{
		std::vector<EntityID> Add;
		std::vector<EntityID> Remove;

		inline void Clear()
		{
			Add.clear();
			Remove.clear();
		}
	} m_Primitives;

	std::unordered_map<EntityID, size_t> m_NodeIDCommandMap;
	Array<std::vector<std::shared_ptr<MeshDrawCommand>>, EGeometryPass> m_Commands;

	std::vector<std::unordered_map<MaterialID, uint32_t>> m_MeshBatch;

	std::mutex m_CommandsLock;
	//TaskEventPtr m_CommandsEvent;
	bool m_AsyncMeshDrawCommandsBuilding;

	static Array<std::unique_ptr<struct MeshDrawCommandBuilder>, EGeometryPass> s_Builders;
};

namespace SceneTextures
{
	const char* const SceneColor = "SceneColor";
	const char* const SceneDepth = "SceneDepth";
};
