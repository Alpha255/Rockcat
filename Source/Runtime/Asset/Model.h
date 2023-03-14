#pragma once

#include "Colorful/IRenderer/IRenderStates.h"
#include "Runtime/Scene/SceneNode.h"
#include "Runtime/Asset/Material.h"

struct MeshAttributes
{
public:
	uint32_t NumVertex() const
	{
		return VertexCount;
	}

	uint32_t NumIndex() const
	{
		return IndexCount;
	}

	uint32_t NumFace() const
	{
		return FaceCount;
	}

	uint32_t VertexStride() const
	{
		return Stride;
	}

	AABB BoundingBox() const
	{
		return AxisAlignedBoundingBox;
	}

	RHI::EPrimitiveTopology PrimitiveTopology() const
	{
		return PrimitiveTopologyType;
	}

	RHI::EIndexFormat IndexFormat() const
	{
		return IndexFormatType;
	}
protected:
	uint32_t VertexCount = 0u;
	uint32_t IndexCount = 0u;
	uint32_t FaceCount = 0u;
	uint32_t Stride = 0u;
	AABB AxisAlignedBoundingBox;
	RHI::EPrimitiveTopology PrimitiveTopologyType = RHI::EPrimitiveTopology::TriangleList;
	RHI::EIndexFormat IndexFormatType = RHI::EIndexFormat::UInt32;
};

class Mesh : public MeshAttributes
{
public:
	struct VertexLayout
	{
	public:
		enum ELimits
		{
			MaxColors = 1,
			MaxUVs = 2
		};

		VertexLayout(uint32_t NumVertex, uint32_t NumIndex, bool8_t HasNormal, bool8_t HasTangent, uint32_t NumUV = 1u, uint32_t NumColor = 0u)
			: VertexCount(NumVertex)
			, IndexCount(NumIndex)
			, HasNormal(HasNormal)
			, HasTangent(HasTangent)
			, HasBitangent(HasTangent)
			, IndexFormat(NumVertex >= std::numeric_limits<uint16_t>::max() ? RHI::EIndexFormat::UInt32 : RHI::EIndexFormat::UInt16)
			, UVs(NumUV)
			, Colors(NumColor)
		{
			assert(NumUV <= ELimits::MaxUVs);
			assert(NumColor <= ELimits::MaxColors);

			size_t CumulateStride = Align(sizeof(Vector3), alignof(Color));
			if (HasNormal)
			{
				CumulateStride += Align(sizeof(Vector3), alignof(Color));
			}
			if (HasTangent)
			{
				CumulateStride += Align(sizeof(Vector3), alignof(Color));
			}
			if (HasBitangent)
			{
				CumulateStride += Align(sizeof(Vector3), alignof(Color));
			}
			for (uint32_t Index = 0u; Index < NumUV; ++Index)
			{
				CumulateStride += Align(sizeof(Vector3), alignof(Color));
			}
			for (uint32_t Index = 0u; Index < NumColor; ++Index)
			{
				CumulateStride += Align(sizeof(Color), alignof(Color));
			}

			if (NumVertex > 0u)
			{
				Vertices = std::shared_ptr<byte8_t>(new byte8_t[VertexCount * CumulateStride]());
			}

			if (NumIndex > 0u)
			{
				Indices = std::shared_ptr<byte8_t>(new byte8_t[IndexCount * static_cast<size_t>(IndexFormat)]());
			}

			Stride = static_cast<uint32_t>(CumulateStride);
		}

		void SetPosition(uint32_t Index, const Vector3& Position)
		{
			SetVertexData(Index, OffsetOfPosition(), Position);
		}

		void SetNormal(uint32_t Index, const Vector3& Normal)
		{
			assert(HasNormal);
			SetVertexData(Index, OffsetOfNormal(), Normal);
		}

		void SetTangent(uint32_t Index, const Vector3& Tangent)
		{
			assert(HasTangent);
			SetVertexData(Index, OffsetOfTangent(), Tangent);
		}

		void SetBitangent(uint32_t Index, const Vector3& Bitangent)
		{
			assert(HasBitangent);
			SetVertexData(Index, OffsetOfBitangent(), Bitangent);
		}

		void SetUV(uint32_t Index, uint32_t UVIndex, const Vector3& UV)
		{
			SetVertexData(Index, OffsetOfUVs(UVIndex), UV);
		}

		void SetColor(uint32_t Index, uint32_t ColorIndex, const Color& Color)
		{
			SetVertexData(Index, OffsetOfColors(ColorIndex), Color);
		}

		void SetFace(uint32_t FaceIndex, uint32_t Index0, uint32_t Index1, uint32_t Index2)
		{
			assert(IndexCount > 0u);

			if (IndexFormat == RHI::EIndexFormat::UInt16)
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

		RHI::InputLayoutDesc ToInputLayoutDesc() const;
	protected:
		template<class Vector>
		void SetVertexData(uint32_t Index, size_t Offset, const Vector& Vertex)
		{
			auto Ptr = Vertices.get() + Index * Stride + Offset;
			*(reinterpret_cast<Vector*>(Ptr)) = Vertex;
		}

		constexpr size_t OffsetOfPosition() const
		{
			return 0;
		}

		constexpr size_t OffsetOfNormal() const
		{
			return OffsetOfPosition() + Align(sizeof(Vector3), alignof(Color));
		}

		constexpr size_t OffsetOfTangent() const
		{
			return HasNormal ? OffsetOfNormal() + Align(sizeof(Vector3), alignof(Color)) : OffsetOfNormal();
		}

		constexpr size_t OffsetOfBitangent() const
		{
			return HasTangent ? OffsetOfTangent() + Align(sizeof(Vector3), alignof(Color)) : OffsetOfTangent();
		}

		constexpr size_t OffsetOfUVs(uint32_t UVIndex) const
		{
			assert(UVIndex < UVs);
			return (HasBitangent ? OffsetOfBitangent() + Align(sizeof(Vector3), alignof(Color)) : OffsetOfBitangent()) + UVIndex * Align(sizeof(Vector3), alignof(Color));
		}

		constexpr size_t OffsetOfColors(uint32_t ColorIndex) const
		{
			assert(ColorIndex < Colors);
			return OffsetOfUVs(UVs) + ColorIndex * Align(sizeof(Color), alignof(Color));
		}

	public:
		uint32_t VertexCount = 0u;
		uint32_t IndexCount = 0u;
		uint32_t Stride = 0u;
		bool8_t HasNormal = false;
		bool8_t HasTangent = false;
		bool8_t HasBitangent = false;
		RHI::EIndexFormat IndexFormat = RHI::EIndexFormat::UInt16;
		uint32_t UVs = 1u;
		uint32_t Colors = 0u;
		std::shared_ptr<byte8_t> Vertices;
		std::shared_ptr<byte8_t> Indices;
	};

	Mesh() = default;

	Mesh(const char8_t* Name)
		: m_DebugName(Name)
	{
	}

	Mesh(
		const Mesh::VertexLayout& Layout, 
		RHI::EPrimitiveTopology Topology,
		const RHI::ShaderDesc& Desc,
		const AABB& BoundingBox = AABB(), 
		const char8_t* Name = nullptr);

	virtual ~Mesh() = default;

	void SetDebugName(const char8_t* Name)
	{
		m_DebugName = Name;
	}

	const char8_t* DebugName() const
	{
		return m_DebugName.c_str();
	}

	RHI::IInputLayout* InputLayout() const
	{
		assert(m_InputLayout);
		return m_InputLayout.get();
	}

	RHI::IBuffer* VertexBuffer()
	{
		return m_VertexBuffer.get();
	}

	RHI::IBuffer* IndexBuffer()
	{
		return m_IndexBuffer.get();
	}
protected:
	friend struct std::hash<VertexLayout>;
	friend class AssimpImporter;
private:
	RHI::IBufferPtr m_VertexBuffer;
	RHI::IBufferPtr m_IndexBuffer;
	RHI::IInputLayoutPtr m_InputLayout;

	std::string m_DebugName;
};

class MeshInstance : public SceneNode
{
public:
	MeshInstance(const std::shared_ptr<Mesh>& Mesh, const std::shared_ptr<Material>& Material)
		: m_Mesh(Mesh)
		, m_Material(Material)
	{
		assert(Mesh);
	}

	MeshInstance(const MeshInstance& Other)
		: SceneNode(Other)
		, m_Mesh(Other.m_Mesh)
		, m_Material(Other.m_Material)
	{
		assert(Other.m_Mesh);
	}

	MeshInstance& operator=(const MeshInstance& Other)
	{
		SceneNode::operator=(Other);
		m_Mesh = Other.m_Mesh;
		m_Material = Other.m_Material;

		return *this;
	}

	void SetMaterial(const std::shared_ptr<Material>& Material)
	{
		m_Material = Material;
	}

	void SetMaterial(Material* Material)
	{
		m_Material.reset(Material);
	}

	Mesh* GetMesh() const
	{
		return m_Mesh.get();
	}

	Material* GetMaterial() const
	{
		return m_Material.get();
	}

	static std::shared_ptr<MeshInstance> FullscreenTriangle();

	static std::shared_ptr<MeshInstance> FullscreenQuad();
protected:
private:
	std::shared_ptr<Mesh> m_Mesh;
	std::shared_ptr<Material> m_Material;
};

class Model : public MeshAttributes
{
public:
	enum class ETextureType
	{
		Diffuse,
		Specular,
		Ambient,
		Emissive,
		Height,
		Normal,
		Shininess,
		Opacity,
		Displacement,
		Lightmap,
		Reflection,
		Albedo,
		Normal_Camera,
		Emission_Color,
		Metalness,
		Diffuse_Roughness,
		Ambient_Occlusion,
		Unknown
	};

	const std::vector<MeshInstance>& Meshes() const
	{
		return m_Meshes;
	}
protected:
	struct LoadingSettings
	{
		bool8_t MakeLeftHanded = true;
		bool8_t GenBoundingBoxes = true;
	};

	friend class AssimpImporter;
private:
	std::vector<MeshInstance> m_Meshes;
};

using ModelAsset = Asset<Model, IAsset::ECategory::Model>;

class ModelInstance : public SceneNode
{
public:
	ModelInstance(const char8_t* Path);

	ModelInstance(const std::shared_ptr<ModelAsset>& Model)
		: SceneNode(EType::Model)
		, m_Model(Model)
	{
		assert(m_Model);
	}

	ModelInstance(const ModelInstance& Other)
		: SceneNode(Other)
		, m_Model(Other.m_Model)
	{
	}

	Model* GetModel() const
	{
		assert(m_Model);
		return m_Model->Object.get();
	}

	std::shared_ptr<ModelInstance> Clone() const
	{
		assert(m_Model);
		return std::make_shared<ModelInstance>(*this);
	}

	void Reimport(const char8_t* Path);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(SceneNode),
			CEREAL_NVP(m_Model)
		);
	}
protected:
	friend class cereal::access;
	friend class Scene;

	ModelInstance()
		: SceneNode(EType::Model)
	{
	}
private:
	std::shared_ptr<ModelAsset> m_Model;
};