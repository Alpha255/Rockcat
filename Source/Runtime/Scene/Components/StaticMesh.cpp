#include "Scene/Components/StaticMesh.h"
#include "RHI/RHIDevice.h"
#include "RHI/RHICommandListContext.h"

MeshData::MeshData(
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
	const char* const Name)
	: MeshProperty(
		NumVertex, 
		NumIndex, 
		NumPrimitive, 
		HasNormal, 
		HasTangent,
		HasUV0, 
		HasUV1, 
		HasColor,
		NumVertex >= std::numeric_limits<uint16_t>::max() ? ERHIIndexFormat::UInt32 : ERHIIndexFormat::UInt16,
		PrimitiveTopology, 
		BoundingBox,
		Name)
{
	assert(NumVertex && NumIndex && NumPrimitive);

	VerticesData.Size += PositionStride * NumVertex; // Position
	VerticesData.Size += HasNormal ? NormalStride * NumVertex : 0u; // Normal
	VerticesData.Size += HasTangent ? TangentStride * NumVertex * 2u : 0u; // Tangent and BiTangent
	VerticesData.Size += HasUV0 ? UVStride * NumVertex : 0u; // UV0
	VerticesData.Size += HasUV1 ? UVStride * NumVertex : 0u; // UV1
	VerticesData.Size += HasColor ? ColorStride * NumVertex : 0u; // Color
	VerticesData.Data.reset(new std::byte[VerticesData.Size]());

	IndicesData.Size += NumIndex * static_cast<size_t>(GetIndexFormat());
	IndicesData.Data.reset(new std::byte[IndicesData.Size]());
}

RHIInputLayoutDesc MeshProperty::GetInputLayout(EVertexAttributes Attributes, ERHIVertexInputRate InputRate)
{
	RHIInputLayoutDesc Desc;
	uint32_t Binding = 0u;
	uint32_t Location = 0u;

	auto AddAttribute = [&Desc, &Binding, &Location, Attributes, InputRate](EVertexAttributes Attribute, size_t Stride, ERHIFormat Format, const char* Usage) {
		if ((Attributes & Attribute) == Attribute)
		{
			Desc.AddBinding(Binding++, Stride, InputRate)
				.AddAttribute(Location++, Stride, Format, Usage);
		}
	};

	AddAttribute(EVertexAttributes::Position, PositionStride, ERHIFormat::RGB32_Float, "POSITION");
	AddAttribute(EVertexAttributes::Normal, NormalStride, ERHIFormat::RGB32_Float, "NORMAL");
	AddAttribute(EVertexAttributes::Tangent, TangentStride, ERHIFormat::RGB32_Float, "TANGENT");
	AddAttribute(EVertexAttributes::Tangent, TangentStride, ERHIFormat::RGB32_Float, "BITANGENT");
	AddAttribute(EVertexAttributes::UV0, UVStride, ERHIFormat::RGB32_Float, "TEXCOORD0");
	AddAttribute(EVertexAttributes::UV1, UVStride, ERHIFormat::RGB32_Float, "TEXCOORD1");
	AddAttribute(EVertexAttributes::Color, ColorStride, ERHIFormat::RGBA32_Float, "COLOR");

	return Desc;
}

StaticMesh::StaticMesh(const MeshProperty& Properties)
	: MeshProperty(Properties)
{
}

const RHIBuffer* StaticMeshBuffers::GetVertexBuffer(EVertexAttributes Attributes) const
{
	switch (Attributes)
	{
	case EVertexAttributes::Position:
		return m_VertexBuffers[0u].get();
	case EVertexAttributes::Normal:
		return m_VertexBuffers[1u].get();
	case EVertexAttributes::Tangent:
		return m_VertexBuffers[2u].get();
	case EVertexAttributes::UV0:
		return m_VertexBuffers[3u].get();
	case EVertexAttributes::UV1:
		return m_VertexBuffers[4u].get();
	case EVertexAttributes::Color:
		return m_VertexBuffers[5u].get();
	}
	return nullptr;
}

void StaticMeshBuffers::CreateRHI(const MeshData& Data, RHIDevice& Device)
{
	RHIBufferDesc Desc;
	if (Data.GetNumIndex())
	{
		assert(Data.IndicesData.Data);

		Desc.SetUsages(ERHIBufferUsageFlags::IndexBuffer)
			.SetAccessFlags(ERHIDeviceAccessFlags::GpuRead)
			.SetPermanentStates(ERHIResourceState::IndexBuffer)
			.SetSize(Data.GetIndexDataSize())
			.SetInitialData(Data.IndicesData.Data.get())
			.SetName(StringUtils::Format("%s-IndexBuffer", Data.GetName()));
		m_IndexBuffer = Device.CreateBuffer(Desc);
	}

	if (Data.GetNumVertex())
	{
		assert(Data.VerticesData.Data);

		Desc.SetUsages(ERHIBufferUsageFlags::VertexBuffer)
			.SetPermanentStates(ERHIResourceState::VertexBuffer)
			.SetSize(Data.GetPositionDataSize())
			.SetInitialData(Data.VerticesData.Data.get() + Data.GetPositionOffset())
			.SetName(StringUtils::Format("%s-PositionBuffer", Data.GetName()));
		m_VertexBuffers[0u] = Device.CreateBuffer(Desc);

		auto CreateVerteBuffer = [&Data, &Device, &Desc, this](bool ShouldCreate, size_t Size, size_t Offset, size_t Index, std::string&& Name) {
			if (ShouldCreate)
			{
				Desc.SetSize(Size)
					.SetInitialData(Data.VerticesData.Data.get() + Offset)
					.SetName(std::move(Name));
				m_VertexBuffers[Index] = Device.CreateBuffer(Desc);
			}
		};

		CreateVerteBuffer(Data.HasNormal(), Data.GetNormalDataSize(), Data.GetNormalOffset(), 1u, 
			StringUtils::Format("%s-NormalBuffer", Data.GetName()));

		CreateVerteBuffer(Data.HasTangent(), Data.GetTangentDataSize(), Data.GetTangentOffset(), 2u,
			StringUtils::Format("%s-TangentBuffer", Data.GetName()));

		CreateVerteBuffer(Data.HasUV0(), Data.GetUV0DataSize(), Data.GetUV0Offset(), 3u,
			StringUtils::Format("%s-UV0Buffer", Data.GetName()));

		CreateVerteBuffer(Data.HasUV1(), Data.GetUV1DataSize(), Data.GetUV1Offset(), 4u,
			StringUtils::Format("%s-UV1Buffer", Data.GetName()));

		CreateVerteBuffer(Data.HasColor(), Data.GetColorDataSize(), Data.GetColorOffset(), 5u,
			StringUtils::Format("%s-TangentBuffer", Data.GetName()));
	}
}
