#pragma once

#include "Engine/Rendering/RenderGraph/RenderPassField.h"
#include "Engine/RHI/RHIRenderStates.h"
#include "Engine/RHI/RHIPipeline.h"
#include "Engine/View/View.h"
#include "Engine/Asset/MaterialAsset.h"

enum class EGeometryPassFilter
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
	std::vector<VertexStream> VertexStreams;
	const RHIBuffer* IndexBuffer = nullptr;

	RHIGraphicsPipeline* GraphicsPipeline = nullptr;

	uint32_t FirstIndex = 0u;
	uint32_t NumPrimitives = 0u;
	uint32_t NumIndex = 0u;
	uint32_t NumInstances = 1u;
	
	MaterialID Material;

	ERHIIndexFormat IndexFormat = ERHIIndexFormat::UInt16;
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;

	union
	{
		struct
		{
			uint32_t BaseIndex;
			uint32_t NumVertex;
		} VertexArgs;

		struct
		{
			RHIBuffer* Buffer;
			uint32_t Offset;
		} IndirectArgs;
	};

	MeshDrawCommand(const class StaticMesh& Mesh);

	inline void AddVertexStream(uint16_t Location, uint16_t Offset, const RHIBuffer* Buffer)
	{
		assert(Buffer);
		VertexStreams.emplace_back(VertexStream{ Location, Offset, Buffer });
	}
};

class RenderScene
{
public:
	void BuildMeshDrawCommands(const class Scene& InScene, class RHIDevice& Device, bool Async);

	const std::vector<MeshDrawCommand>& GetMeshDrawCommands(EGeometryPassFilter MeshPass) const { return m_MeshDrawCommands[(size_t)MeshPass]; }
	const std::vector<std::shared_ptr<IView>>& GetViews() const { return m_Views; }

	void RegisterMeshDrawCommandBuilder(EGeometryPassFilter Filter, struct IGeometryPassMeshDrawCommandBuilder* Builder);
protected:
private:
	std::array<std::vector<MeshDrawCommand>, (size_t)EGeometryPassFilter::Num> m_MeshDrawCommands;
	std::array<std::shared_ptr<struct IGeometryPassMeshDrawCommandBuilder>, (size_t)EGeometryPassFilter::Num> m_MeshDrawCommandBuilders;
	mutable std::vector<std::shared_ptr<IView>> m_Views;
};

namespace SceneTextures
{
	const char* const SceneColor = "SceneColor";
	const char* const SceneDepth = "SceneDepth";
};
