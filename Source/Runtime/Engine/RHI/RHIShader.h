#pragma once

#include "Runtime/Engine/RHI/RHIResource.h"

enum class ERHIShaderLanguage : uint8_t
{
	GLSL,
	HLSL
};

enum class ERHIShaderStage : uint8_t
{
	Vertex,
	Hull,
	Domain,
	Geometry,
	Fragment,
	Compute,
	Num
};


enum class ERHIVertexInputRate : uint8_t
{
	Vertex,
	Instance,
};

struct RHIInputLayoutCreateInfo : public RHIHashedObject
{
	struct RHIVertexAttribute
	{
		uint32_t Location = 0u;
		uint32_t Stride = 0u;

		ERHIFormat Format = ERHIFormat::Unknown;
		std::string Usage;
	};

	struct RHIVertexInputBinding
	{
		uint32_t Binding = 0u;
		uint32_t Stride = 0u;
		ERHIVertexInputRate InputRate = ERHIVertexInputRate::Vertex;

		std::vector<RHIVertexAttribute> Attributes;

		inline RHIVertexInputBinding& AddAttribute(const RHIVertexAttribute& Attribute)
		{
			Stride += Attribute.Stride;
			Attributes.emplace_back(Attribute);
			return *this;
		}
	};

	std::vector<RHIVertexInputBinding> Bindings;

	size_t Hash() const
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(Bindings.size());
			for (auto& Binding : Bindings)
			{
				HashCombine(HashValue, ComputeHash(Binding.Binding, Binding.Stride, Binding.InputRate));

				for (auto& Attr : Binding.Attributes)
				{
					HashCombine(HashValue, ComputeHash(Attr.Location, Attr.Stride, Attr.Format));
				}
			}
		}
		return HashValue;
	}
};

class RHIInputLayout : public RHIResource
{
public:
	using RHIResource::RHIResource;
};

struct RHIShaderCreateInfo
{
	ERHIShaderStage ShaderStage = ERHIShaderStage::Num;
	ERHIShaderLanguage Language = ERHIShaderLanguage::HLSL;

	class ShaderBinary* const Binary = nullptr;
	std::string Name;
};

class RHIShader : RHIResource
{
public:
	using RHIResource::RHIResource;

	ERHIShaderStage GetStage() const { return m_Stage; }
private:
	ERHIShaderStage m_Stage = ERHIShaderStage::Num;
};


class RHIGlobalShader : public RHIShader
{
public:
	using RHIShader::RHIShader;
};