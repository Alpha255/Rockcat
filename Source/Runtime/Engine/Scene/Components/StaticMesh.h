#include "Runtime/Engine/RHI/RHIBuffer.h"
#include "Runtime/Core/Math/AABB.h"
#include "Runtime/Engine/Asset/MaterialAsset.h"

struct Mesh
{
	uint32_t VertexNum = 0u;
	uint32_t IndexNum = 0u;
	uint32_t PrimitiveNum = 0u;

	ERHIIndexFormat IndexFormat = ERHIIndexFormat::UInt16;
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;

	MaterialID Material;

	Math::AABB BoundingBox;

	RHIBufferPtr VertexBuffer;
	RHIBufferPtr IndexBuffer;

	std::string Name;
};

class StaticMesh : public Mesh
{
};

class SkeletalMesh : public Mesh
{
};