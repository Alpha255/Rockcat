#include "Engine/Scene/Components/StaticMesh.h"
#include "Engine/RHI/RHIDevice.h"
#include "Engine/RHI/RHICommandListContext.h"

RHIInputLayoutCreateInfo MeshData::GetInputLayoutCreateInfo() const
{
	return GetPackedInputLayoutCreateInfo(VertexAttributes);
}

MeshData::MeshData(uint32_t InNumVertex, uint32_t InNumIndex, uint32_t InNumPrimitive, bool InHasNormal, bool InHasTangent,
	bool InHasUV0, bool InHasUV1, bool InHasColor, ERHIPrimitiveTopology InPrimitiveTopology,
	const Math::AABB& InBoundingBox, const char* const InName)
	: MeshProperty(
		InNumVertex, InNumIndex, InNumPrimitive, InHasNormal, InHasTangent,
		InHasUV0, InHasUV1, InHasColor,
		InNumVertex >= std::numeric_limits<uint16_t>::max() ? ERHIIndexFormat::UInt32 : ERHIIndexFormat::UInt16,
		InPrimitiveTopology, InBoundingBox)
	, Name(InName ? InName : "")
{
	assert(NumVertex && NumIndex && NumPrimitive);

	size_t Stride = 0u;
	Stride += PositionStride; // Position
	Stride += HasNormal() ? NormalStride : 0u; // Normal
	Stride += HasTangent() ? TangentAndBiTangentStride : 0u; // Tangent and BiTangent
	Stride += HasUV0() ? UVStride : 0u; // UV0
	Stride += HasUV1() ? UVStride : 0u; // UV1
	Stride += HasColor() ? ColorStride : 0u; // Color
	PackedVertexStride = static_cast<uint32_t>(Stride);

	PackedVerticesData.reset(new uint8_t[NumVertex * PackedVertexStride]());
	IndicesData.reset(new uint8_t[NumIndex * static_cast<size_t>(IndexFormat)]());

#define ALLOC_DATA(Exists, Data, Stride) if (Exists) { Data.reset(new uint8_t[NumVertex * Stride]()); }
	ALLOC_DATA(true, PositionData, PositionStride);
	ALLOC_DATA(HasNormal(), NormalData, NormalStride);
	ALLOC_DATA(HasTangent(), TangentData, TangentAndBiTangentStride);
	ALLOC_DATA(HasUV0(), UV0Data, UVStride);
	ALLOC_DATA(HasUV1(), UV1Data, UVStride);
	ALLOC_DATA(HasColor(), ColorData, ColorStride);
#undef ALLOC_DATA
}

void MeshData::ClearData()
{
	PackedVerticesData.reset();
	PositionData.reset();
	NormalData.reset();
	TangentData.reset();
	UV0Data.reset();
	UV1Data.reset();
	ColorData.reset();
	IndicesData.reset();
}

RHIInputLayoutCreateInfo MeshData::GetInputLayoutCreateInfo(EVertexAttributes Attributes, ERHIVertexInputRate InputRate)
{
	RHIInputLayoutCreateInfo CreateInfo;
	uint32_t Binding = 0u;
	uint32_t Location = 0u;

	auto AddAttribute = [&CreateInfo, &Binding, &Location, Attributes, InputRate](EVertexAttributes Attribute, size_t Stride, ERHIFormat Format, const char* Usage) {
		if ((Attributes & Attribute) == Attribute)
		{
			auto& VertexBinding = CreateInfo.AddBinding(Binding++, Stride, InputRate)
				.AddAttribute(Location++, Stride, Format, Usage);

			if (Attribute == EVertexAttributes::Tangent)
			{
				VertexBinding.AddAttribute(Location++, TangentStride, ERHIFormat::RGB32_Float, "BITANGENT");
			}
		}
	};

	AddAttribute(EVertexAttributes::Position, PositionStride, ERHIFormat::RGB32_Float, "POSITION");
	AddAttribute(EVertexAttributes::Normal, NormalStride, ERHIFormat::RGB32_Float, "NORMAL");
	AddAttribute(EVertexAttributes::Tangent, TangentStride, ERHIFormat::RGB32_Float, "TANGENT");
	AddAttribute(EVertexAttributes::UV0, UVStride, ERHIFormat::RGB32_Float, "TEXCOORD0");
	AddAttribute(EVertexAttributes::UV1, UVStride, ERHIFormat::RGB32_Float, "TEXCOORD1");
	AddAttribute(EVertexAttributes::Color, ColorStride, ERHIFormat::RGBA32_Float, "COLOR");

	return CreateInfo;
}

RHIInputLayoutCreateInfo MeshData::GetPackedInputLayoutCreateInfo(EVertexAttributes Attributes, ERHIVertexInputRate InputRate)
{
	RHIInputLayoutCreateInfo CreateInfo;
	uint32_t Location = 0u;

	auto& Binding = CreateInfo.AddBinding(0u, 0u, InputRate);

	auto AddAttribute = [&Binding, &Location, Attributes](EVertexAttributes Attribute, size_t Stride, ERHIFormat Format, const char* Usage) {
		if ((Attributes & Attribute) == Attribute)
		{
			Binding.AddAttribute(Location++, Stride, Format, Usage);

			if (Attribute == EVertexAttributes::Tangent)
			{
				Binding.AddAttribute(Location++, TangentStride, ERHIFormat::RGB32_Float, "BITANGENT");
			}
		}
	};

	AddAttribute(EVertexAttributes::Position, PositionStride, ERHIFormat::RGB32_Float, "POSITION");
	AddAttribute(EVertexAttributes::Normal, NormalStride, ERHIFormat::RGB32_Float, "NORMAL");
	AddAttribute(EVertexAttributes::Tangent, TangentStride, ERHIFormat::RGB32_Float, "TANGENT");
	AddAttribute(EVertexAttributes::UV0, UVStride, ERHIFormat::RGB32_Float, "TEXCOORD0");
	AddAttribute(EVertexAttributes::UV1, UVStride, ERHIFormat::RGB32_Float, "TEXCOORD1");
	AddAttribute(EVertexAttributes::Color, ColorStride, ERHIFormat::RGBA32_Float, "COLOR");

	return CreateInfo;
}

StaticMesh::StaticMesh(const MeshData& Data, MaterialID ID)
	: MeshData(Data)
	, m_MaterialID(ID)
{
}

std::vector<const RHIBuffer*> StaticMesh::GetVertexBuffers(EVertexAttributes Attributes) const
{
	std::vector<const RHIBuffer*> Buffers;
	Buffers.reserve(static_cast<size_t>(EVertexAttributes::Num));

	auto AddBuffer = [this, Attributes, &Buffers](EVertexAttributes Attribute) {
		if ((Attributes & Attribute) == Attribute)
		{
			const size_t Index = GetPowerOfTwo(static_cast<size_t>(Attribute));
			assert(Index < RHIVertexBuffers.size() && RHIVertexBuffers[Index]);
			Buffers.emplace_back(RHIVertexBuffers[Index].get());
		}
	};

	AddBuffer(EVertexAttributes::Position);
	AddBuffer(EVertexAttributes::Normal);
	AddBuffer(EVertexAttributes::Tangent);
	AddBuffer(EVertexAttributes::UV0);
	AddBuffer(EVertexAttributes::UV1);
	AddBuffer(EVertexAttributes::Color);

	return Buffers;
}

const RHIBuffer* StaticMesh::GetVertexBuffer(EVertexAttributes Attributes) const
{
	/// TODO: Pack all buffer into one ???
	const size_t Index = GetPowerOfTwo(static_cast<size_t>(Attributes));
	switch (Attributes)
	{
	case EVertexAttributes::Position:
	case EVertexAttributes::Normal:
	case EVertexAttributes::Tangent:
	case EVertexAttributes::UV0:
	case EVertexAttributes::UV1:
	case EVertexAttributes::Color:
		return RHIVertexBuffers[Index].get();
	}

	return nullptr;
}

void StaticMesh::SetVertexBuffer(EVertexAttributes Attributes, RHIBufferPtr&& Buffer)
{
	const size_t Index = GetPowerOfTwo(static_cast<size_t>(Attributes));
	switch (Attributes)
	{
	case EVertexAttributes::Position:
	case EVertexAttributes::Normal:
	case EVertexAttributes::Tangent:
	case EVertexAttributes::UV0:
	case EVertexAttributes::UV1:
	case EVertexAttributes::Color:
		RHIVertexBuffers[Index] = std::move(Buffer);
	}
}

void StaticMesh::CreateRHI(RHIDevice& Device)
{
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
}
