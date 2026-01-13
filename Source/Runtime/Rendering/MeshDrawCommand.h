#pragma once

#include "RHI/RHIBuffer.h"
#include "RHI/RHIRenderStates.h"
#include "RHI/RHIPipeline.h"
#include "Asset/Material.h"

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
	RHIGraphicsPipelineDesc PipelineDesc;

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

	const char* GetDebugName() const {
		return Material ? Material->Name.c_str() : "Unknown";
	}
};
