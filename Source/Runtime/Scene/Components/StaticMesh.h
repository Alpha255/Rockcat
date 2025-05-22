#pragma once

#include "RHI/RHIBuffer.h"
#include "RHI/RHIShader.h"
#include "Core/Math/AABB.h"
#include "Core/Math/Sphere.h"
#include "Core/Math/Color.h"
#include "Core/Math/Transform.h"
#include "Asset/Material.h"

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

class MeshProperty
{
public:
	MeshProperty(
		uint32_t NumVertex, 
		uint32_t NumIndex, 
		uint32_t NumPrimitive, 
		bool HasNormal, 
		bool HasTangent,
		bool HasUV0, 
		bool HasUV1, 
		bool HasColor, 
		ERHIIndexFormat IndexFormat, 
		ERHIPrimitiveTopology PrimitiveTopology,
		const Math::AABB& BoundingBox,
		const char* const Name)
		: m_NumVertex(NumVertex)
		, m_NumIndex(NumIndex)
		, m_NumPrimitive(NumPrimitive)
		, m_IndexFormat(IndexFormat)
		, m_PrimitiveTopology(PrimitiveTopology)
		, m_BoundingBox(BoundingBox)
		, m_BoundingSphere(
			BoundingBox.GetCenter(),
			std::max<float>(std::max<float>(BoundingBox.GetExtents().x, BoundingBox.GetExtents().y), BoundingBox.GetExtents().z))
		, m_Name(Name ? Name : "")
	{
		EVertexAttributes None = static_cast<EVertexAttributes>(0u);

		m_VertexAttributes = m_VertexAttributes | (HasNormal ? EVertexAttributes::Normal : None);
		m_VertexAttributes = m_VertexAttributes | (HasTangent ? EVertexAttributes::Tangent : None);
		m_VertexAttributes = m_VertexAttributes | (HasUV0 ? EVertexAttributes::UV0 : None);
		m_VertexAttributes = m_VertexAttributes | (HasUV1 ? EVertexAttributes::UV1 : None);
		m_VertexAttributes = m_VertexAttributes | (HasColor ? EVertexAttributes::Color : None);
	}

	MeshProperty(const MeshProperty&) = default;
	MeshProperty& operator=(const MeshProperty&) = default;

	inline bool HasNormal() const { return HasAttribute(EVertexAttributes::Normal); }
	inline bool HasTangent() const { return HasAttribute(EVertexAttributes::Tangent); }
	inline bool HasUV0() const { return HasAttribute(EVertexAttributes::UV0); }
	inline bool HasUV1() const { return HasAttribute(EVertexAttributes::UV1); }
	inline bool HasColor() const { return HasAttribute(EVertexAttributes::Color); }

	inline uint32_t GetNumVertex() const { return m_NumVertex; }
	inline uint32_t GetNumIndex() const { return m_NumIndex; }
	inline uint32_t GetNumPrimitive() const { return m_NumPrimitive; }

	inline ERHIIndexFormat GetIndexFormat() const { return m_IndexFormat; }
	inline ERHIPrimitiveTopology GetPrimitiveTopology() const { return m_PrimitiveTopology; }

	inline const Math::AABB& GetBoundingBox() const { return m_BoundingBox; }
	inline const Math::Sphere& GetBoundingSphere() const { return m_BoundingSphere; }

	inline MaterialID GetMaterialID() const { return m_MaterialID; }

	inline const char* GetName() const { return m_Name.c_str(); }

	inline RHIInputLayoutCreateInfo GetInputLayout() const { return GetInputLayout(m_VertexAttributes); }

	static RHIInputLayoutCreateInfo GetInputLayout(EVertexAttributes Attributes, ERHIVertexInputRate InputRate = ERHIVertexInputRate::Vertex);

	static const size_t PositionStride = sizeof(Math::Vector3);
	static const size_t NormalStride = sizeof(Math::Vector3);
	static const size_t TangentStride = sizeof(Math::Vector3);
	static const size_t UVStride = sizeof(Math::Vector3);
	static const size_t ColorStride = sizeof(Math::Color);
	static const size_t AlignOf = alignof(Math::Vector4);

	static_assert(AlignOf == sizeof(float));
private:
	inline bool HasAttribute(EVertexAttributes Attribute) const { return (m_VertexAttributes & Attribute) == Attribute; }
	inline void SetMaterialID(MaterialID ID) { m_MaterialID = ID; }

	uint32_t m_NumVertex = 0u;
	uint32_t m_NumIndex = 0u;
	uint32_t m_NumPrimitive = 0u;
	MaterialID m_MaterialID = ~0u;

	EVertexAttributes m_VertexAttributes = EVertexAttributes::Position;
	ERHIIndexFormat m_IndexFormat = ERHIIndexFormat::UInt16;
	ERHIPrimitiveTopology m_PrimitiveTopology = ERHIPrimitiveTopology::TriangleList;

	Math::AABB m_BoundingBox;
	Math::Sphere m_BoundingSphere;

	std::string m_Name;
};

struct MeshData : public MeshProperty
{
	MeshData(
		uint32_t NumVertex, 
		uint32_t NumIndex, 
		uint32_t NumPrimitive, 
		bool HasNormal, 
		bool HasTangent,
		bool HasUV0, 
		bool HasUV1, 
		bool HasColor, 
		ERHIPrimitiveTopology PrimitiveTopology,
		const Math::AABB& BoundingBox, 
		const char* const Name = nullptr);

	MeshData(const MeshData&) = default;
	MeshData& operator=(const MeshData&) = default;

	inline void SetPosition(uint32_t Index, const Math::Vector3& Position)
	{
		SetVertexData(Index, GetPositionOffset(), Position);
	}
	inline void SetNormal(uint32_t Index, const Math::Vector3& Normal)
	{ 
		assert(HasNormal());
		SetVertexData(Index, GetNormalOffset(), Normal);
	}
	inline void SetTangent(uint32_t Index, const Math::Vector3& Tangent)
	{
		assert(HasTangent());
		SetVertexData(Index, GetTangentOffset(), Tangent);
	}
	inline void SetBitangent(uint32_t Index, const Math::Vector3& Bitangent) 
	{ 
		assert(HasTangent());
		SetVertexData(Index, GetBiTangentOffset(), Bitangent);
	}
	inline void SetUV0(uint32_t Index, const Math::Vector3& UV) 
	{
		assert(HasUV0());
		SetVertexData(Index, GetUV0Offset(), UV);
	}
	inline void SetUV1(uint32_t Index, const Math::Vector3& UV) 
	{
		assert(HasUV1());
		SetVertexData(Index, GetUV1Offset(), UV);
	}
	inline void SetColor(uint32_t Index, const Math::Color& Color) 
	{
		assert(HasColor());
		SetVertexData(Index, GetColorOffset(), Color);
	}

	inline void SetFace(uint32_t FaceIndex, uint32_t Index0, uint32_t Index1, uint32_t Index2)
	{
		if (GetIndexFormat() == ERHIIndexFormat::UInt16)
		{
			auto IndexDataPtr = reinterpret_cast<uint16_t*>(IndicesData.Data.get()) + FaceIndex * 3u;
			IndexDataPtr[0] = static_cast<uint16_t>(Index0);
			IndexDataPtr[1] = static_cast<uint16_t>(Index1);
			IndexDataPtr[2] = static_cast<uint16_t>(Index2);
		}
		else
		{
			auto IndexDataPtr = reinterpret_cast<uint32_t*>(IndicesData.Data.get()) + FaceIndex * 3u;
			IndexDataPtr[0] = Index0;
			IndexDataPtr[1] = Index1;
			IndexDataPtr[2] = Index2;
		}
	}

	template<class Vector, size_t Stride = sizeof(Vector)>
	inline void SetVertexData(uint32_t Index, size_t Offset, const Vector& Value)
	{
		*(reinterpret_cast<Vector*>(VerticesData.Data.get() + Offset + Index * Stride)) = Value;
	}

	inline constexpr size_t GetIndexDataSize() const { return GetNumIndex() * static_cast<size_t>(GetIndexFormat()); }

	inline constexpr size_t GetPositionDataSize() const { return PositionStride * GetNumVertex(); }
	inline constexpr size_t GetNormalDataSize() const { return HasNormal() ? (NormalStride * GetNumVertex()) : 0u; }
	inline constexpr size_t GetTangentDataSize() const { return HasTangent() ? (TangentStride * GetNumVertex()) : 0u; }
	inline constexpr size_t GetUV0DataSize() const { return HasUV0() ? (UVStride * GetNumVertex()) : 0u; }
	inline constexpr size_t GetUV1DataSize() const { return HasUV1() ? (UVStride * GetNumVertex()) : 0u; }
	inline constexpr size_t GetColorDataSize() const { return HasColor() ? (ColorStride * GetNumVertex()) : 0u; }

	inline constexpr size_t GetPositionOffset() const { return 0u; }
	inline constexpr size_t GetNormalOffset() const { return GetPositionOffset() + GetPositionDataSize(); }
	inline constexpr size_t GetTangentOffset() const { return GetNormalOffset() + GetNormalDataSize(); }
	inline constexpr size_t GetBiTangentOffset() const { return GetTangentOffset() + GetTangentDataSize(); }
	inline constexpr size_t GetUV0Offset() const { return GetBiTangentOffset() + GetTangentDataSize(); }
	inline constexpr size_t GetUV1Offset() const { return GetUV0Offset() + GetUV0DataSize(); }
	inline constexpr size_t GetColorOffset() const { return GetUV1Offset() + GetUV1DataSize(); }

	DataBlock VerticesData;
	DataBlock IndicesData;
};

class StaticMeshBuffers
{
public:
	const RHIBuffer* GetIndexBuffer() const { return m_IndexBuffer.get(); }
	const RHIBuffer* GetPositionBuffer() const { return GetVertexBuffer(EVertexAttributes::Position); }
	const RHIBuffer* GetNormalBuffer() const { return GetVertexBuffer(EVertexAttributes::Normal); }
	const RHIBuffer* GetTangentBuffer() const { return GetVertexBuffer(EVertexAttributes::Tangent); }
	const RHIBuffer* GetUV0Buffer() const { return GetVertexBuffer(EVertexAttributes::UV0); }
	const RHIBuffer* GetUV1Buffer() const { return GetVertexBuffer(EVertexAttributes::UV1); }
	const RHIBuffer* GetColorBuffer() const { return GetVertexBuffer(EVertexAttributes::Color); }
	const RHIBuffer* GetVertexBuffer(EVertexAttributes Attributes) const;
private:
	virtual void CreateRHI(const MeshData& Data, class RHIDevice& Device);

	Array<RHIBufferPtr, EVertexAttributes> m_VertexBuffers;
	RHIBufferPtr m_IndexBuffer;
};

class StaticMesh : public MeshProperty, public StaticMeshBuffers
{
protected:
	friend class AssimpSceneImporter;

	StaticMesh(const MeshProperty& Properties);
};

class SkinnedMeshBuffers : public StaticMeshBuffers
{
};

class SkinnedMesh : public StaticMesh
{
};