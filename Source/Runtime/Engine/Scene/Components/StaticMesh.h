#pragma once

#include "Engine/RHI/RHIBuffer.h"
#include "Engine/RHI/RHIShader.h"
#include "Core/Math/AABB.h"
#include "Core/Math/Sphere.h"
#include "Core/Math/Color.h"
#include "Engine/Asset/Material.h"

enum class EVertexAttributes : uint8_t
{
	Position = 1 << 0,
	Normal = 1 << 1,
	Tangent = 1 << 2,
	UV0 = 1 << 3,
	UV1 = 1 << 4,
	Color = 1 << 5,
	Num = 6
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
};

struct MeshData : public MeshProperty
{
	MeshData(uint32_t InNumVertex, uint32_t InNumIndex, uint32_t InNumPrimitive, bool InHasNormal, bool InHasTangent,
		bool InHasUV0, bool InHasUV1, bool InHasColor, ERHIPrimitiveTopology InPrimitiveTopology,
		const Math::AABB& InBoundingBox, const char* const InName = nullptr);

	MeshData(const MeshData&) = default;
	MeshData& operator=(const MeshData&) = default;

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
		SetData<Math::Vector3, TangentAndBiTangentStride>(Index, TangentData, Tangent);
	}
	inline void SetBitangent(uint32_t Index, const Math::Vector3& Bitangent) 
	{ 
		assert(HasTangent());
		SetPackedData(Index, GetBiTangentOffset(), Bitangent);
		SetData<Math::Vector3, TangentAndBiTangentStride>(Index, TangentData, Bitangent, TangentStride);
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
	static RHIInputLayoutCreateInfo GetInputLayoutCreateInfo(EVertexAttributes Attributes, ERHIVertexInputRate InputRate = ERHIVertexInputRate::Vertex);
	static RHIInputLayoutCreateInfo GetPackedInputLayoutCreateInfo(EVertexAttributes Attributes, ERHIVertexInputRate InputRate = ERHIVertexInputRate::Vertex);

	template<class Vector>
	inline void SetPackedData(uint32_t Index, size_t Offset, const Vector& Value)
	{
		*(reinterpret_cast<Vector*>(PackedVerticesData.get() + Index * PackedVertexStride + Offset)) = Value;
	}

	template<class Vector, size_t Stride = sizeof(Vector)>
	inline void SetData(uint32_t Index, std::shared_ptr<uint8_t>& Data, const Vector& Value, size_t Offset = 0u)
	{
		*(reinterpret_cast<Vector*>(Data.get() + Index * Stride + Offset)) = Value;
	}

	static const size_t PositionStride = sizeof(Math::Vector3);
	static const size_t NormalStride = sizeof(Math::Vector3);
	static const size_t TangentStride = sizeof(Math::Vector3);
	static const size_t TangentAndBiTangentStride = sizeof(Math::Vector3) * 2u;
	static const size_t UVStride = sizeof(Math::Vector3);
	static const size_t ColorStride = sizeof(Math::Color);
	static const size_t AlignOf = alignof(Math::Vector4);

	static_assert(AlignOf == sizeof(float));

	constexpr size_t GetPositionOffset() const { return 0u; }
	constexpr size_t GetNormalOffset() const { return GetPositionOffset() + PositionStride; }
	constexpr size_t GetTangentOffset() const { return HasNormal() ? (GetNormalOffset() + NormalStride) : GetNormalOffset(); }
	constexpr size_t GetBiTangentOffset() const { return GetTangentOffset() + TangentStride; }
	constexpr size_t GetUV0Offset() const { return HasTangent() ? (GetBiTangentOffset() + TangentStride) : GetTangentOffset(); }
	constexpr size_t GetUV1Offset() const { return HasUV0() ? (GetUV0Offset() + UVStride) : GetUV0Offset(); }
	constexpr size_t GetColorOffset() const { return GetUV1Offset() + UVStride; }

	void ClearData();

	virtual void CreateRHI(class RHIDevice&) {}

	std::shared_ptr<uint8_t> PackedVerticesData;
	std::shared_ptr<uint8_t> PositionData;
	std::shared_ptr<uint8_t> NormalData;
	std::shared_ptr<uint8_t> TangentData;
	std::shared_ptr<uint8_t> UV0Data;
	std::shared_ptr<uint8_t> UV1Data;
	std::shared_ptr<uint8_t> ColorData;

	std::shared_ptr<uint8_t> IndicesData;

	RHIBufferPtr RHIPackedVertexBuffer;
	RHIBufferPtr RHIIndexBuffer;
	std::array<RHIBufferPtr, (size_t)EVertexAttributes::Num> RHIVertexBuffers;

	const MaterialProperty* Material = nullptr;

	std::string Name;
};

class StaticMesh : private MeshData
{
public:
	StaticMesh(const MeshData& Data, MaterialID ID);

	inline uint32_t GetNumVertex() const { return NumVertex; }
	inline uint32_t GetNumIndex() const { return NumIndex; }
	inline uint32_t GetNumPrimitive() const { return NumPrimitive; }
	inline uint32_t GetPackedVertexStride() const { return PackedVertexStride; }
	inline const Math::AABB& GetBoundingBox() const { return BoundingBox; }
	inline const Math::Sphere& GetBoundingSphere() const { return BoundingSphere; }
	inline ERHIPrimitiveTopology GetPrimitiveTopology() const { return PrimitiveTopology; }
	inline ERHIIndexFormat GetIndexFormat() const { return IndexFormat; }
	inline const char* const GetName() const { return Name.c_str(); }

	inline MaterialID GetMaterialID() const { return m_MaterialID; }
	inline const MaterialProperty& GetMaterialProperty() const { assert(Material); return *Material; }

	inline const RHIBuffer* GetPackedVertexBuffer() const { assert(RHIPackedVertexBuffer);  return RHIPackedVertexBuffer.get(); }
	inline const RHIBuffer* GetIndexBuffer() const { assert(RHIIndexBuffer);  return RHIIndexBuffer.get(); }
	std::vector<const RHIBuffer*> GetVertexBuffers(EVertexAttributes Attributes) const;
	const RHIBuffer* GetVertexBuffer(EVertexAttributes Attributes) const;

	void CreateRHI(class RHIDevice& Device) override;
protected:
	friend class SceneBuilder;
	friend class AssimpSceneImporter;

	void SetMaterialID(MaterialID ID) { m_MaterialID = ID; }
	void SetMaterialProperty(const MaterialProperty* InMaterial) { Material = InMaterial; }
	void SetVertexBuffer(EVertexAttributes Attributes, RHIBufferPtr&& Buffer);

	MaterialID m_MaterialID;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(MeshProperty),
			CEREAL_NVP(m_MaterialID)
		);
	}
};

class SkeletalMesh : public StaticMesh
{
};