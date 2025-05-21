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

RHIInputLayoutCreateInfo MeshProperty::GetInputLayout(EVertexAttributes Attributes, ERHIVertexInputRate InputRate)
{
	RHIInputLayoutCreateInfo CreateInfo;
	uint32_t Binding = 0u;
	uint32_t Location = 0u;

	auto AddAttribute = [&CreateInfo, &Binding, &Location, Attributes, InputRate](EVertexAttributes Attribute, size_t Stride, ERHIFormat Format, const char* Usage) {
		if ((Attributes & Attribute) == Attribute)
		{
			CreateInfo.AddBinding(Binding++, Stride, InputRate)
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

	return CreateInfo;
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
#if 0
	if (RHIIndexBuffer || RHIPackedVertexBuffer)
	{
		return;
	}

	RHIBufferCreateInfo CreateInfo;
	if (NumIndex)
	{
		assert(IndicesData);
		CreateInfo.SetUsages(ERHIBufferUsageFlags::IndexBuffer)
			.SetAccessFlags(ERHIDeviceAccessFlags::GpuRead)
			.SetPermanentStates(ERHIResourceState::IndexBuffer)
			.SetSize(NumIndex * static_cast<size_t>(IndexFormat))
			.SetInitialData(IndicesData.get())
			.SetName(StringUtils::Format("%s-IndexBuffer", GetName()));
		RHIIndexBuffer = Device.CreateBuffer(CreateInfo);
	}

	if (NumVertex)
	{
		assert(PositionData);
		CreateInfo.SetUsages(ERHIBufferUsageFlags::VertexBuffer)
			.SetPermanentStates(ERHIResourceState::VertexBuffer)
			.SetSize(PositionStride * NumVertex)
			.SetInitialData(PositionData.get())
			.SetName(StringUtils::Format("%s-PositionBuffer", GetName()));
		SetVertexBuffer(EVertexAttributes::Position, Device.CreateBuffer(CreateInfo));

		if (HasNormal())
		{
			assert(NormalData);
			CreateInfo.SetSize(NormalStride * NumVertex)
				.SetInitialData(NormalData.get())
				.SetName(StringUtils::Format("%s-NormalBuffer", GetName()));
			SetVertexBuffer(EVertexAttributes::Normal, Device.CreateBuffer(CreateInfo));
		}

		if (HasTangent())
		{
			assert(TangentData);
			CreateInfo.SetSize(TangentAndBiTangentStride * NumVertex)
				.SetInitialData(TangentData.get())
				.SetName(StringUtils::Format("%s-TangentBuffer", GetName()));
			SetVertexBuffer(EVertexAttributes::Tangent, Device.CreateBuffer(CreateInfo));
		}

		if (HasUV0())
		{
			assert(UV0Data);
			CreateInfo.SetSize(UVStride * NumVertex)
				.SetInitialData(UV0Data.get())
				.SetName(StringUtils::Format("%s-UV0Buffer", GetName()));
			SetVertexBuffer(EVertexAttributes::UV0, Device.CreateBuffer(CreateInfo));
		}

		if (HasUV1())
		{
			assert(UV1Data);
			CreateInfo.SetSize(UVStride * NumVertex)
				.SetInitialData(UV1Data.get())
				.SetName(StringUtils::Format("%s-UV1Buffer", GetName()));
			SetVertexBuffer(EVertexAttributes::UV1, Device.CreateBuffer(CreateInfo));
		}
	}

	ClearData();
#endif
}
