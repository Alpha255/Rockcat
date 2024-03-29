#pragma once

#include "Engine/RHI/RHIBuffer.h"
#include "Engine/RHI/RHIShader.h"
#include "Core/Math/AABB.h"
#include "Core/Math/Sphere.h"
#include "Core/Math/Color.h"
#include "Engine/Asset/MaterialAsset.h"

struct MeshProperty
{
	MeshProperty() = default;

	MeshProperty(uint32_t InNumVertex, uint32_t InNumIndex, uint32_t InNumPrimitive, bool InHasTangent,
		bool InHasUV0, bool InHasUV1, bool InHasColor, ERHIIndexFormat InIndexFormat, ERHIPrimitiveTopology InPrimitiveTopology,
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
			std::max<float>(std::max<float>(InBoundingBox.GetExtents().x, InBoundingBox.GetExtents().y), InBoundingBox.GetExtents().z))
	{
	}

	MeshProperty(const MeshProperty&) = default;
	MeshProperty& operator=(const MeshProperty&) = default;

	uint32_t NumVertex = 0u;
	uint32_t NumIndex = 0u;
	uint32_t NumPrimitive = 0u;
	uint32_t VertexStride = 0u;

	bool HasTangent = false;
	bool HasUV0 = false;
	bool HasUV1 = false;
	bool HasColor = false;

	ERHIIndexFormat IndexFormat = ERHIIndexFormat::UInt16;
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;

	Math::AABB BoundingBox;
	Math::Sphere BoundingSphere;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(NumVertex),
			CEREAL_NVP(NumIndex),
			CEREAL_NVP(NumPrimitive),
			CEREAL_NVP(VertexStride),
			CEREAL_NVP(HasTangent),
			CEREAL_NVP(HasUV0),
			CEREAL_NVP(HasUV1),
			CEREAL_NVP(HasColor),
			CEREAL_NVP(IndexFormat),
			CEREAL_NVP(PrimitiveTopology),
			CEREAL_NVP(BoundingBox),
			CEREAL_NVP(BoundingSphere)
		);
	}
};

struct MeshData : public MeshProperty
{
	MeshData(uint32_t InNumVertex, uint32_t InNumIndex, uint32_t InNumPrimitive, bool InHasTangent,
		bool InHasUV0, bool InHasUV1, bool InHasColor, ERHIPrimitiveTopology InPrimitiveTopology,
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

		Vertices.reset(new uint8_t[NumVertex * VertexStride]());
		Indices.reset(new uint8_t[NumIndex * static_cast<size_t>(IndexFormat)]());
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

	std::unique_ptr<uint8_t> Vertices;
	std::unique_ptr<uint8_t> Indices;
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

	const RHIBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
	const RHIBuffer* GetIndexBuffer() const { return m_IndexBuffer.get(); }

	MaterialID GetMaterialID() const { return m_Material; }
private:
protected:
	friend class SceneBuilder;
	void SetMaterialID(MaterialID ID) { m_Material = ID; }

	MaterialID m_Material;

	RHIBufferPtr m_VertexBuffer;
	RHIBufferPtr m_IndexBuffer;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(MeshProperty),
			CEREAL_NVP(m_Material)
		);
	}
};

class SkeletalMesh : public StaticMesh
{
};