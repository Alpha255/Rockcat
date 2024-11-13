#pragma once

#include "Engine/Rendering/RenderGraph/RenderPassField.h"
#include "Engine/RHI/RHIRenderStates.h"
#include "Engine/RHI/RHIPipeline.h"
#include "Engine/View/View.h"

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
	uint16_t Index = 0;
	uint16_t Offset = 0u;
	const RHIBuffer* VertexBuffer = nullptr;
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

	const class MaterialAsset* Material = nullptr;

	GraphicsPipelineKey GfxPipelineKey;

	uint32_t FirstIndex = 0u;
	uint32_t NumPrimitives = 0u;
	uint32_t NumIndex = 0u;
	uint32_t NumInstances = 1u;

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

	MeshDrawCommand(const class StaticMesh& Mesh, const class MaterialAsset* InMaterial, class GeometryPassGraphicsPipelineBuilder* PipelineBuilder);
};

class GeometryPassGraphicsPipelineBuilder
{
public:
	virtual void Build(MeshDrawCommand& Command) = 0;
};

class RenderScene
{
public:
	RenderScene();

	void BuildMeshDrawCommands(const class Scene& InScene, bool AsyncBuilding);

	const std::vector<MeshDrawCommand>& GetMeshDrawCommands(EGeometryPassFilter MeshPass) const { return m_MeshDrawCommands[(size_t)MeshPass]; }
	const std::vector<std::shared_ptr<IView>>& GetViews() const;
protected:
private:
	std::array<std::vector<MeshDrawCommand>, (size_t)EGeometryPassFilter::Num> m_MeshDrawCommands;
	std::array<std::unique_ptr<GeometryPassGraphicsPipelineBuilder>, (size_t)EGeometryPassFilter::Num> m_PipelineBuilders;
	mutable std::vector<std::shared_ptr<IView>> m_Views;
};
