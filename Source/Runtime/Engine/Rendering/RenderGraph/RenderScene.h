#pragma once

#include "Engine/Rendering/RenderGraph/RenderPassField.h"
#include "Engine/RHI/RHIRenderStates.h"
#include "Engine/RHI/RHIPipeline.h"
#include "Engine/Asset/Material.h"
#include "Engine/Scene/SceneGraph.h"
#include "Engine/Scene/SceneView.h"

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
	enum class ECullFlags
	{
		None,
		CulledByFrustum,
		CulledByOcclusion
	};

	RHIVertexStream VertexStream;
	const RHIBuffer* IndexBuffer = nullptr;

	uint32_t FirstIndex = 0u;
	uint32_t NumPrimitives = 0u;
	uint32_t NumIndex = 0u;
	uint32_t NumInstances = 1u;
	ERHIIndexFormat IndexFormat = ERHIIndexFormat::UInt16;

	std::string_view DebugName;
	
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
};

class RenderScene
{
public:
	RenderScene(const class Scene& InScene);

	const class Scene& GetScene() const { return m_Scene; }
	const std::vector<std::shared_ptr<SceneView>>& GetViews() const { return m_Views; }
	const std::vector<std::shared_ptr<MeshDrawCommand>>& GetCommands(EGeometryPass Filter) const { return m_Commands[Filter]; }

	void BuildMeshDrawCommands(const struct RenderSettings& GraphicsSettings);
	void WaitCommandsBuilding();

	static void RegisterMeshDrawCommandBuilder(EGeometryPass Filter, struct MeshDrawCommandBuilder* Builder);
protected:
	template<class Index>
	inline struct MeshDrawCommandBuilder* GetBuilder(Index Filter) { return s_Builders[Filter].get(); }
private:
	void GetScenePrimitives();
	void UpdateScenePrimitives();
	void RemoveInvalidCommands();

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

	std::vector<std::shared_ptr<SceneView>> m_Views;

	std::unordered_map<SceneGraph::NodeID, size_t> m_NodeIDCommandMap;
	Array<std::vector<std::shared_ptr<MeshDrawCommand>>, EGeometryPass> m_Commands;

	std::mutex m_CommandsMutex;
	TaskEventPtr m_CommandsEvent;

	static Array<std::unique_ptr<struct MeshDrawCommandBuilder>, EGeometryPass> s_Builders;
};

namespace SceneTextures
{
	const char* const SceneColor = "SceneColor";
	const char* const SceneDepth = "SceneDepth";
};
