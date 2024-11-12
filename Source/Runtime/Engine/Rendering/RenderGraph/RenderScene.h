#pragma once

#include "Engine/Rendering/RenderGraph/RenderPassField.h"
#include "Engine/RHI/RHIRenderStates.h"
#include "Engine/View/View.h"

enum class EGeometryPassFilter
{
	PreDepth,
	Opaque,
	Translucent,
	ShadowCaster,
	Num
};

struct VertexStream
{
	uint16_t Index = 0;
	uint16_t Offset = 0u;
	RHIBuffer* VertexBuffer = nullptr;
};

struct GraphicsPipelineKey
{
	size_t Hash = 0u;
	RHIGraphicsPipeline* RHI = nullptr;
	// Shader Resource Binding
};

struct MeshDrawCommand
{
	std::vector<VertexStream> VertexStreams;
	RHIBuffer* IndexBuffer = nullptr;

	GraphicsPipelineKey GfxPipelineKey;

	uint32_t FirstIndex = 0u;
	uint32_t NumPrimitives = 0u;
	uint32_t NumInstances = 1u;

	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;

	union
	{
		struct
		{
			uint32_t BaseVertexIndex;
			uint32_t NumVertices;
		} VertexArgs;

		struct
		{
			RHIBuffer* Buffer;
			uint32_t Offset;
		} IndirectArgs;
	};
};

class RenderScene
{
public:
	RenderScene(const class Scene& InScene)
	{
		GenerateDrawCommands(InScene);
	}

	const std::vector<const MeshDrawCommand*>& GetDrawCommands(EGeometryPassFilter MeshPass) const { return m_MeshDrawCommands[(size_t)MeshPass]; }
	const std::vector<std::shared_ptr<IView>>& GetViews() const;
protected:
private:
	void GenerateDrawCommands(const class Scene& InScene);

	std::array<std::vector<const MeshDrawCommand*>, (size_t)EGeometryPassFilter::Num> m_MeshDrawCommands;
	std::vector<MeshDrawCommand> m_DrawCommands;
	mutable std::vector<std::shared_ptr<IView>> m_Views;
};
