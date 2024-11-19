#pragma once

#include "Engine/RHI/RHIBuffer.h"
#include "Engine/RHI/RHIShader.h"
#include "Core/Math/AABB.h"
#include "Core/Math/Sphere.h"
#include "Core/Math/Color.h"
#include "Engine/Asset/MaterialAsset.h"

enum class EVertexAttributes : uint8_t
{
	Position = 1 << 0,
	Normal = 1 << 1,
	Tangent = 1 << 2,
	UV0 = 1 << 3,
	UV1 = 1 << 4,
	Color = 1 << 5
};
ENUM_FLAG_OPERATORS(EVertexAttributes);

struct MeshProperty
{
	MeshProperty() = default;

	MeshProperty(uint32_t InNumVertex, uint32_t InNumIndex, uint32_t InNumPrimitive, bool InHasNormal, bool InHasTangent,
		bool InHasUV0, bool InHasUV1, bool InHasColor, ERHIIndexFormat InIndexFormat, ERHIPrimitiveTopology InPrimitiveTopology,
		const Math::AABB& InBoundingBox)
		: NumVertex(InNumVertex)
		, NumIndex(InNumIndex)
		, NumPrimitive(InNumPrimitive)
		, IndexFormat(InIndexFormat)
		, PrimitiveTopology(InPrimitiveTopology)
		, BoundingBox(InBoundingBox)
		, BoundingSphere(
			InBoundingBox.GetCenter(), 
			std::max<float>(std::max<float>(InBoundingBox.GetExtents().x, InBoundingBox.GetExtents().y), InBoundingBox.GetExtents().z))
	{
		EVertexAttributes None = static_cast<EVertexAttributes>(0u);

		VertexAttributes = VertexAttributes | (InHasNormal ? EVertexAttributes::Normal : None);
		VertexAttributes = VertexAttributes | (InHasTangent ? EVertexAttributes::Tangent : None);
		VertexAttributes = VertexAttributes | (InHasUV0 ? EVertexAttributes::UV0 : None);
		VertexAttributes = VertexAttributes | (InHasUV1 ? EVertexAttributes::UV1 : None);
		VertexAttributes = VertexAttributes | (InHasColor ? EVertexAttributes::Color : None);
	}

	MeshProperty(const MeshProperty&) = default;
	MeshProperty& operator=(const MeshProperty&) = default;

	uint32_t NumVertex = 0u;
	uint32_t NumIndex = 0u;
	uint32_t NumPrimitive = 0u;
	uint32_t PackedVertexStride = 0u;

	EVertexAttributes VertexAttributes = EVertexAttributes::Position;
	ERHIIndexFormat IndexFormat = ERHIIndexFormat::UInt16;
	ERHIPrimitiveTopology PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;

	Math::AABB BoundingBox;
	Math::Sphere BoundingSphere;

	inline bool HasAttribute(EVertexAttributes Attribute) const { return (VertexAttributes & Attribute) == Attribute; }
	inline bool HasNormal() const { return HasAttribute(EVertexAttributes::Normal); }
	inline bool HasTangent() const { return HasAttribute(EVertexAttributes::Tangent); }
	inline bool HasUV0() const { return HasAttribute(EVertexAttributes::UV0); }
	inline bool HasUV1() const { return HasAttribute(EVertexAttributes::UV1); }
	inline bool HasColor() const { return HasAttribute(EVertexAttributes::Color); }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(NumVertex),
			CEREAL_NVP(NumIndex),
			CEREAL_NVP(NumPrimitive),
			CEREAL_NVP(PackedVertexStride),
			CEREAL_NVP(VertexAttributes),
			CEREAL_NVP(IndexFormat),
			CEREAL_NVP(PrimitiveTopology),
			CEREAL_NVP(BoundingBox),
			CEREAL_NVP(BoundingSphere)
		);
	}

private:
	virtual void CrateRHIBuffers(const struct MeshData&) {}
};

struct MeshData : public MeshProperty
{
	MeshData(uint32_t InNumVertex, uint32_t InNumIndex, uint32_t InNumPrimitive, bool InHasNormal, bool InHasTangent,
		bool InHasUV0, bool InHasUV1, bool InHasColor, ERHIPrimitiveTopology InPrimitiveTopology,
		const Math::AABB& InBoundingBox)
		: MeshProperty(
			InNumVertex, InNumIndex, InNumPrimitive, InHasNormal, InHasTangent, 
			InHasUV0, InHasUV1, InHasColor,
			InNumVertex >= std::numeric_limits<uint16_t>::max() ? ERHIIndexFormat::UInt32 : ERHIIndexFormat::UInt16,
			InPrimitiveTopology, InBoundingBox)
	{
		assert(NumVertex && NumIndex && NumPrimitive);

		size_t Stride = 0u;
		Stride += Align(sizeof(Math::Vector3), alignof(Math::Vector4)); // Position
		Stride += HasNormal() ? Align(sizeof(Math::Vector3), alignof(Math::Vector4)) : 0u; // Normal
		Stride += HasTangent() ? Align(sizeof(Math::Vector3) + sizeof(Math::Vector3), alignof(Math::Vector4)) : 0u; // Tangent and BiTangent
		Stride += HasUV0() ? Align(sizeof(Math::Vector3), alignof(Math::Vector4)) : 0u; // UV0
		Stride += HasUV1() ? Align(sizeof(Math::Vector3), alignof(Math::Vector4)) : 0u; // UV1
		Stride += HasColor() ? Align(sizeof(Math::Color), alignof(Math::Color)) : 0u; // Color
		PackedVertexStride = static_cast<uint32_t>(Stride);

		PackedVerticesData.reset(new uint8_t[NumVertex * PackedVertexStride]());
		IndicesData.reset(new uint8_t[NumIndex * static_cast<size_t>(IndexFormat)]());
	}

	inline void SetPosition(uint32_t Index, const Math::Vector3& Position)
	{
		SetPackedData(Index, GetPositionOffset(), Position);
		SetData(Index, PositionData, Position);
	}
	inline void SetNormal(uint32_t Index, const Math::Vector3& Normal)
	{ 
		assert(HasNormal());
		SetPackedData(Index, GetNormalOffset(), Normal);
		SetData(Index, NormalData, Normal);
	}
	inline void SetTangent(uint32_t Index, const Math::Vector3& Tangent)
	{
		assert(HasTangent());
		SetPackedData(Index, GetTangentOffset(), Tangent);
		SetData<Math::Vector3, sizeof(Math::Vector3) * 2u>(Index, TangentData, Tangent);
	}
	inline void SetBitangent(uint32_t Index, const Math::Vector3& Bitangent) 
	{ 
		assert(HasTangent());
		SetPackedData(Index, GetBiTangentOffset(), Bitangent);
		SetData<Math::Vector3, sizeof(Math::Vector3) * 2u>(Index, TangentData, Bitangent, sizeof(Math::Vector3));
	}
	inline void SetUV0(uint32_t Index, const Math::Vector3& UV) 
	{
		assert(HasUV0());
		SetPackedData(Index, GetUV0Offset(), UV);
		SetData(Index, UV0Data, UV);
	}
	inline void SetUV1(uint32_t Index, const Math::Vector3& UV) 
	{
		assert(HasUV1());
		SetPackedData(Index, GetUV1Offset(), UV);
		SetData(Index, UV1Data, UV);
	}
	inline void SetColor(uint32_t Index, const Math::Color& Color) 
	{
		assert(HasColor());
		SetPackedData(Index, GetColorOffset(), Color);
		SetData(Index, ColorData, Color);
	}

	inline void SetFace(uint32_t FaceIndex, uint32_t Index0, uint32_t Index1, uint32_t Index2)
	{
		if (IndexFormat == ERHIIndexFormat::UInt16)
		{
			auto IndexDataPtr = reinterpret_cast<uint16_t*>(IndicesData.get()) + FaceIndex * 3u;
			IndexDataPtr[0] = static_cast<uint16_t>(Index0);
			IndexDataPtr[1] = static_cast<uint16_t>(Index1);
			IndexDataPtr[2] = static_cast<uint16_t>(Index2);
		}
		else
		{
			auto IndexDataPtr = reinterpret_cast<uint32_t*>(IndicesData.get()) + FaceIndex * 3u;
			IndexDataPtr[0] = Index0;
			IndexDataPtr[1] = Index1;
			IndexDataPtr[2] = Index2;
		}
	}

	RHIInputLayoutCreateInfo GetInputLayoutCreateInfo() const;
	static RHIInputLayoutCreateInfo GetInputLayoutCreateInfo(EVertexAttributes Attributes);
	static RHIInputLayoutCreateInfo GetPackedInputLayoutCreateInfo(EVertexAttributes Attributes);

	template<class Vector>
	inline void SetPackedData(uint32_t Index, size_t Offset, const Vector& Value)
	{
		*(reinterpret_cast<Vector*>(PackedVerticesData.get() + Index * PackedVertexStride + Offset)) = Value;
	}

	template<class Vector, size_t Stride = sizeof(Vector)>
	inline void SetData(uint32_t Index, std::unique_ptr<uint8_t>& Data, const Vector& Value, size_t Offset = 0u)
	{
		*(reinterpret_cast<Vector*>(Data.get() + Index * Stride + Offset)) = Value;
	}

	constexpr size_t GetPositionOffset() const { return 0u; }
	constexpr size_t GetNormalOffset() const { return GetPositionOffset() + Align(sizeof(Math::Vector3), alignof(Math::Vector4)); }
	constexpr size_t GetTangentOffset() const { return GetNormalOffset() + Align(sizeof(Math::Vector3), alignof(Math::Vector4)); }
	constexpr size_t GetBiTangentOffset() const { return GetTangentOffset() + Align(sizeof(Math::Vector3), alignof(Math::Vector4)); }
	constexpr size_t GetUV0Offset() const { return HasTangent() ? (GetBiTangentOffset() + Align(sizeof(Math::Vector3), alignof(Math::Vector4))) : GetTangentOffset(); }
	constexpr size_t GetUV1Offset() const { return HasUV0() ? (GetUV0Offset() + Align(sizeof(Math::Vector3), alignof(Math::Vector4))) : GetUV0Offset(); }
	constexpr size_t GetColorOffset() const { return GetUV1Offset() + Align(sizeof(Math::Color), alignof(Math::Color)); }

	std::unique_ptr<uint8_t> PackedVerticesData;
	std::unique_ptr<uint8_t> PositionData;
	std::unique_ptr<uint8_t> NormalData;
	std::unique_ptr<uint8_t> TangentData;
	std::unique_ptr<uint8_t> UV0Data;
	std::unique_ptr<uint8_t> UV1Data;
	std::unique_ptr<uint8_t> ColorData;

	std::unique_ptr<uint8_t> IndicesData;
};

class StaticMesh : private MeshProperty
{
public:
	StaticMesh() = default;

	StaticMesh(const MeshData& Data, MaterialID Material);

	uint32_t GetNumVertex() const { return NumVertex; }
	uint32_t GetNumIndex() const { return NumIndex; }
	uint32_t GetNumPrimitive() const { return NumPrimitive; }
	uint32_t GetPackedVertexStride() const { return PackedVertexStride; }
	const Math::AABB& GetBoundingBox() const { return BoundingBox; }
	const Math::Sphere& GetBoundingSphere() const { return BoundingSphere; }
	ERHIPrimitiveTopology GetPrimitiveTopology() const { return PrimitiveTopology; }
	ERHIIndexFormat GetIndexFormat() const { return IndexFormat; }

	//const RHIBuffer* GetVertexBuffer() const { return m_VertexBuffer.get(); }
	const RHIBuffer* GetIndexBuffer() const { return m_IndexBuffer.get(); }

	MaterialID GetMaterialID() const { return m_Material; }
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
private:
	virtual void CrateRHIBuffers(const MeshData& Data) override final;
};

class SkeletalMesh : public StaticMesh
{
};