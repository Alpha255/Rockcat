#include "Runtime/Engine/RHI/RHIBuffer.h"
#include "Runtime/Engine/RHI/RHIShader.h"
#include "Runtime/Core/Math/AABB.h"
#include "Runtime/Core/Math/Sphere.h"
#include "Runtime/Core/Math/Color.h"
#include "Runtime/Engine/Asset/MaterialAsset.h"

struct MeshProperty
{
	MeshProperty() = default;

	MeshProperty(uint32_t InNumVertex, uint32_t InNumIndex, uint32_t InNumPrimitive, bool8_t InHasTangent,
		bool8_t InHasUV0, bool8_t InHasUV1, bool8_t InHasColor, ERHIIndexFormat InIndexFormat, ERHIPrimitiveTopology InPrimitiveTopology,
		const Math::AABB& InBoundingBox)
		: NumVertex(InNumVertex)
		, NumIndex(InNumIndex)
		, NumPrimitive(InNumPrimitive)
		, HasTangent(InHasTangent)
		, HasUV0(InHasUV0)
		, HasUV1(InHasUV1)
		, HasColor(InHasColor)
		, IndexFormat(InIndexFormat)
		, PrimitiveTopology(InPrimitiveTopology)
		, BoundingBox(InBoundingBox)
		, BoundingSphere(
			InBoundingBox.GetCenter(), 
			std::max<float32_t>(std::max<float32_t>(InBoundingBox.GetExtents().x, InBoundingBox.GetExtents().y), InBoundingBox.GetExtents().z))
	{
	}

	MeshProperty(const MeshProperty&) = default;
	MeshProperty& operator=(const MeshProperty&) = default;

	uint32_t NumVertex = 0u;
	uint32_t NumIndex = 0u;
	uint32_t NumPrimitive = 0u;
	uint32_t VertexStride = 0u;

	bool8_t HasTangent = false;
	bool8_t HasUV0 = false;
	bool8_t HasUV1 = false;
	bool8_t HasColor = false;

	ERHIIndexFormat IndexFormat = ERHIIndexFormat::UInt16;
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;

	Math::AABB BoundingBox;
	Math::Sphere BoundingSphere;
};

struct MeshData : public MeshProperty
{
	MeshData(uint32_t InNumVertex, uint32_t InNumIndex, uint32_t InNumPrimitive, bool8_t InHasTangent,
		bool8_t InHasUV0, bool8_t InHasUV1, bool8_t InHasColor, ERHIPrimitiveTopology InPrimitiveTopology,
		const Math::AABB& InBoundingBox)
		: MeshProperty(
			InNumVertex, InNumIndex, InNumPrimitive, InHasTangent, 
			InHasUV0, InHasUV1, InHasColor,
			InNumVertex >= std::numeric_limits<uint16_t>::max() ? ERHIIndexFormat::UInt32 : ERHIIndexFormat::UInt16,
			InPrimitiveTopology, InBoundingBox)
	{
		assert(NumVertex && NumIndex && NumPrimitive);

		size_t Stride = 0u;
		Stride += Align(sizeof(Math::Vector3), alignof(Math::Vector4)); // Position
		Stride += Align(sizeof(Math::Vector3), alignof(Math::Vector4)); // Normal
		Stride += HasTangent ? Align(sizeof(Math::Vector3), alignof(Math::Vector4)) : 0u; // Tangent
		Stride += HasTangent ? Align(sizeof(Math::Vector3), alignof(Math::Vector4)) : 0u; // BiTangent
		Stride += HasUV0 ? Align(sizeof(Math::Vector3), alignof(Math::Vector4)) : 0u; // UV0
		Stride += HasUV1 ? Align(sizeof(Math::Vector3), alignof(Math::Vector4)) : 0u; // UV1
		Stride += HasColor ? Align(sizeof(Math::Color), alignof(Math::Color)) : 0u; // Color
		VertexStride = static_cast<uint32_t>(Stride);

		Vertices.reset(new byte8_t[NumVertex * VertexStride]());
		Indices.reset(new byte8_t[NumIndex * static_cast<size_t>(IndexFormat)]());
	}

	void SetPosition(uint32_t Index, const Math::Vector3& Position) { SetData(Index, OffsetOfPosition(), Position); }
	void SetNormal(uint32_t Index, const Math::Vector3& Normal) { SetData(Index, OffsetOfNormal(), Normal); }
	void SetTangent(uint32_t Index, const Math::Vector3& Tangent) { assert(HasTangent); SetData(Index, OffsetOfTangent(), Tangent); }
	void SetBitangent(uint32_t Index, const Math::Vector3& Bitangent) { assert(HasTangent); SetData(Index, OffsetOfBitangent(), Bitangent); }
	void SetUV0(uint32_t Index, const Math::Vector3& UV) { assert(HasUV0); SetData(Index, OffsetOfUV0(), UV); }
	void SetUV1(uint32_t Index, const Math::Vector3& UV) { assert(HasUV1); SetData(Index, OffsetOfUV1(), UV); }
	void SetColor(uint32_t Index, const Math::Color& Color) { assert(HasColor); SetData(Index, OffsetOfColor(), Color); }

	void SetFace(uint32_t FaceIndex, uint32_t Index0, uint32_t Index1, uint32_t Index2)
	{
		if (IndexFormat == ERHIIndexFormat::UInt16)
		{
			auto IndexDataPtr = reinterpret_cast<uint16_t*>(Indices.get()) + FaceIndex * 3u;
			IndexDataPtr[0] = static_cast<uint16_t>(Index0);
			IndexDataPtr[1] = static_cast<uint16_t>(Index1);
			IndexDataPtr[2] = static_cast<uint16_t>(Index2);
		}
		else
		{
			auto IndexDataPtr = reinterpret_cast<uint32_t*>(Indices.get()) + FaceIndex * 3u;
			IndexDataPtr[0] = Index0;
			IndexDataPtr[1] = Index1;
			IndexDataPtr[2] = Index2;
		}
	}

	RHIInputLayoutCreateInfo GetInputLayoutCreateInfo() const;

	template<class Vector>
	void SetData(uint32_t Index, size_t Offset, const Vector& Data)
	{
		auto Ptr = Vertices.get() + Index * VertexStride + Offset;
		*(reinterpret_cast<Vector*>(Ptr)) = Data;
	}

	constexpr size_t OffsetOfPosition() const { return 0u; }
	constexpr size_t OffsetOfNormal() const { return OffsetOfPosition() + Align(sizeof(Math::Vector3), alignof(Math::Vector4)); }
	constexpr size_t OffsetOfTangent() const { return OffsetOfNormal() + Align(sizeof(Math::Vector3), alignof(Math::Vector4)); }
	constexpr size_t OffsetOfBitangent() const { return OffsetOfTangent() + Align(sizeof(Math::Vector3), alignof(Math::Vector4)); }
	constexpr size_t OffsetOfUV0() const { return HasTangent ? (OffsetOfBitangent() + Align(sizeof(Math::Vector3), alignof(Math::Vector4))) : OffsetOfTangent(); }
	constexpr size_t OffsetOfUV1() const { return HasUV0 ? (OffsetOfUV0() + Align(sizeof(Math::Vector3), alignof(Math::Vector4))) : OffsetOfUV0(); }
	constexpr size_t OffsetOfColor() const { return OffsetOfUV1() + Align(sizeof(Math::Color), alignof(Math::Color)); }

	std::unique_ptr<byte8_t> Vertices;
	std::unique_ptr<byte8_t> Indices;
};

class StaticMesh : private MeshProperty
{
public:
	StaticMesh() = default;

	StaticMesh(const MeshData& Data, MaterialID Material);

	uint32_t GetNumVertex() const { return NumVertex; }
	uint32_t GetNumIndex() const { return NumIndex; }
	uint32_t GetNumPrimitive() const { return NumPrimitive; }
	uint32_t GetVertexStride() const { return VertexStride; }
	const Math::AABB& GetBoundingBox() const { return BoundingBox; }
	const Math::Sphere& GetBoundingSphere() const { return BoundingSphere; }
	ERHIPrimitiveTopology GetPrimitiveTopology() const { return PrimitiveTopology; }
	ERHIIndexFormat GetIndexFormat() const { return IndexFormat; }

	MaterialID GetMaterialID() const { return m_Material; }
private:
	MaterialID m_Material;

	RHIBufferPtr m_VertexBuffer;
	RHIBufferPtr m_IndexBuffer;
};

class SkeletalMesh : public StaticMesh
{
};