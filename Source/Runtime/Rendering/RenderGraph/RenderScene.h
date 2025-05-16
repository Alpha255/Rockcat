#pragma once

#include "Rendering/RenderGraph/RenderPassField.h"
#include "RHI/RHIRenderStates.h"
#include "RHI/RHIPipeline.h"
#include "Asset/Material.h"
#include "Scene/SceneGraph.h"
#include "Scene/SceneView.h"

enum class EGeometryPass
{
	PreDepth,
	Opaque,
	Translucent,
	ShadowCast,
	Num
};

struct VertexStream
{
	uint16_t Location = 0;
	uint16_t Offset = 0u;
	const RHIBuffer* Buffer = nullptr;
};

struct MeshDrawCommand
{
	RHIVertexStream VertexStream;
	const RHIBuffer* IndexBuffer = nullptr;

	uint32_t FirstIndex = 0u;
	uint32_t NumPrimitives = 0u;
	uint32_t NumIndex = 0u;
	uint32_t NumInstances = 1u;
	ERHIIndexFormat IndexFormat = ERHIIndexFormat::UInt16;
	
	const MaterialProperty* Material = nullptr;
	RHIGraphicsPipelineCreateInfo GraphicsPipelineCreateInfo;

	union
	{
		struct
		{
			int32_t BaseIndex;
			uint32_t NumVertex;
		} VertexArgs;

		struct
		{
			RHIBuffer* Buffer;
			uint32_t Offset;
		} IndirectArgs;
	};

	MeshDrawCommand() = default;
	MeshDrawCommand(const class StaticMesh& Mesh);

	const char* GetDebugName() const { return Material ? Material->Name.c_str() : "Unknown"; }
};

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
		std::vector<SceneGraph::NodeID> Add;
		std::vector<SceneGraph::NodeID> Remove;

		inline void Clear()
		{
			Add.clear();
			Remove.clear();
		}
	} m_Primitives;

	std::unordered_map<SceneGraph::NodeID, size_t> m_NodeIDCommandMap;
	Array<std::vector<std::shared_ptr<MeshDrawCommand>>, EGeometryPass> m_Commands;

	std::vector<std::unordered_map<MaterialID, uint32_t>> m_MeshBatch;

	std::mutex m_CommandsLock;
	TaskEventPtr m_CommandsEvent;
	bool m_AsyncMeshDrawCommandsBuilding;

	static Array<std::unique_ptr<struct MeshDrawCommandBuilder>, EGeometryPass> s_Builders;
};

namespace SceneTextures
{
	const char* const SceneColor = "SceneColor";
	const char* const SceneDepth = "SceneDepth";
};
