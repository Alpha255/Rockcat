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

struct GraphicsPipelineKey
{
	size_t Hash = 0u;
	RHIGraphicsPipeline* Pipeline = nullptr;
	RHIShaderResourceBindings ShaderResourceLayout;
};

struct MeshDrawCommand
{
	std::vector<VertexStream> VertexStreams;
	const RHIBuffer* IndexBuffer = nullptr;

	GraphicsPipelineKey GfxPipelineKey;

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

class GeometryPassMeshDrawCommandBuilder
{
public:
	virtual MeshDrawCommand Build(const class StaticMesh& Mesh, const class Scene& InScene) = 0;
};

class RenderScene
{
public:
	RenderScene(const GraphicsSettings& GfxSettings);

	void BuildMeshDrawCommands(const class Scene& InScene);

	const std::vector<MeshDrawCommand>& GetMeshDrawCommands(EGeometryPassFilter MeshPass) const { return m_MeshDrawCommands[(size_t)MeshPass]; }
	const std::vector<std::shared_ptr<IView>>& GetViews() const { return m_Views; }
protected:
private:
	std::array<std::vector<MeshDrawCommand>, (size_t)EGeometryPassFilter::Num> m_MeshDrawCommands;
	std::array<std::unique_ptr<GeometryPassMeshDrawCommandBuilder>, (size_t)EGeometryPassFilter::Num> m_MeshDrawCommandBuilders;
	const GraphicsSettings& m_GfxSettings;
	mutable std::vector<std::shared_ptr<IView>> m_Views;
};
