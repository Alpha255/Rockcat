#include "Engine/Scene/Components/StaticMesh.h"

RHIInputLayoutCreateInfo MeshData::GetInputLayoutCreateInfo() const
{
	return RHIInputLayoutCreateInfo();
}

RHIInputLayoutCreateInfo MeshData::GetInputLayoutCreateInfo(EVertexAttributes Attributes)
{
	RHIInputLayoutCreateInfo CreateInfo;
	uint32_t Binding = 0u;
	uint32_t Location = 0u;

	auto AddAttribute = [&CreateInfo, &Binding, &Location, Attributes](EVertexAttributes Attribute, size_t Stride, ERHIVertexInputRate InputRate, ERHIFormat Format, const char* Usage) {
		if ((Attributes & Attribute) == Attribute)
		{
			auto& VertexBinding = CreateInfo.AddBinding(Binding++, Stride, InputRate)
				.AddAttribute(Location++, Stride, Format, Usage);

			if (Attribute == EVertexAttributes::Tangent)
			{
				VertexBinding.AddAttribute(Location++, sizeof(Math::Vector3), ERHIFormat::RGB32_Float, "BITANGENT");
			}
		}
	};

	AddAttribute(EVertexAttributes::Position, sizeof(Math::Vector3), ERHIVertexInputRate::Vertex, ERHIFormat::RGB32_Float, "POSITION");
	AddAttribute(EVertexAttributes::Normal, sizeof(Math::Vector3), ERHIVertexInputRate::Vertex, ERHIFormat::RGB32_Float, "NORMAL");
	AddAttribute(EVertexAttributes::Tangent, sizeof(Math::Vector3), ERHIVertexInputRate::Vertex, ERHIFormat::RGB32_Float, "TANGENT");
	AddAttribute(EVertexAttributes::UV0, sizeof(Math::Vector3), ERHIVertexInputRate::Vertex, ERHIFormat::RGB32_Float, "TEXCOORD0");
	AddAttribute(EVertexAttributes::UV1, sizeof(Math::Vector3), ERHIVertexInputRate::Vertex, ERHIFormat::RGB32_Float, "TEXCOORD1");
	AddAttribute(EVertexAttributes::Color, sizeof(Math::Color), ERHIVertexInputRate::Vertex, ERHIFormat::RGBA32_Float, "COLOR");

	return CreateInfo;
}

RHIInputLayoutCreateInfo MeshData::GetPackedInputLayoutCreateInfo(EVertexAttributes Attributes)
{
	RHIInputLayoutCreateInfo CreateInfo;
	uint32_t Location = 0u;

	auto& Binding = CreateInfo.AddBinding(0u, 0u, ERHIVertexInputRate::Vertex);

	auto AddAttribute = [&Binding, &Location, Attributes](EVertexAttributes Attribute, size_t Stride, ERHIFormat Format, const char* Usage) {
		if ((Attributes & Attribute) == Attribute)
		{
			Binding.AddAttribute(Location++, Stride, Format, Usage);

			if (Attribute == EVertexAttributes::Tangent)
			{
				Binding.AddAttribute(Location++, sizeof(Math::Vector3), ERHIFormat::RGB32_Float, "BITANGENT");
			}
		}
	};

	AddAttribute(EVertexAttributes::Position, sizeof(Math::Vector3), ERHIFormat::RGB32_Float, "POSITION");
	AddAttribute(EVertexAttributes::Normal, sizeof(Math::Vector3), ERHIFormat::RGB32_Float, "NORMAL");
	AddAttribute(EVertexAttributes::Tangent, sizeof(Math::Vector3), ERHIFormat::RGB32_Float, "TANGENT");
	AddAttribute(EVertexAttributes::UV0, sizeof(Math::Vector3), ERHIFormat::RGB32_Float, "TEXCOORD0");
	AddAttribute(EVertexAttributes::UV1, sizeof(Math::Vector3), ERHIFormat::RGB32_Float, "TEXCOORD1");
	AddAttribute(EVertexAttributes::Color, sizeof(Math::Color), ERHIFormat::RGBA32_Float, "COLOR");

	return CreateInfo;
}

StaticMesh::StaticMesh(const MeshData& Data, MaterialID Material)
	: MeshProperty(Data)
	, m_Material(Material)
{
	CrateRHIBuffers(Data);
}

void StaticMesh::CrateRHIBuffers(const MeshData& Data)
{
}
