#pragma once

#include "RHI/RHIResource.h"

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
	Num,
	Compute,
};
ENUM_FLAG_OPERATORS(ERHIShaderStage)

enum class ERHIVertexInputRate : uint8_t
{
	Vertex,
	Instance,
};

struct RHIInputLayoutDesc
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
			assert(Attribute.Stride);
			Stride += Attribute.Stride;
			Attributes.emplace_back(Attribute);
			return *this;
		}

		template<class T>
		inline RHIVertexInputBinding& AddAttribute(uint32_t InLocation, T InStride, ERHIFormat InFormat, const char* InUsage)
		{
			Stride += static_cast<uint32_t>(InStride);
			Attributes.emplace_back(RHIVertexAttribute
				{
					InLocation,
					static_cast<uint32_t>(InStride),
					InFormat,
					std::string(InUsage)
				});
			return *this;
		}
	};

	inline RHIInputLayoutDesc& AddBinding(const RHIVertexInputBinding& InputBinding)
	{
		assert(InputBinding.Binding < ERHILimitations::MaxVertexStreams);
		Bindings.emplace_back(InputBinding);
		return *this;
	}

	template<class T>
	inline RHIVertexInputBinding& AddBinding(uint32_t InBinding, T InStride, ERHIVertexInputRate InInputRate)
	{
		assert(InBinding < ERHILimitations::MaxVertexStreams);
		return Bindings.emplace_back(RHIVertexInputBinding
			{
				InBinding,
				static_cast<uint32_t>(InStride),
				InInputRate
			});
	}

	std::vector<RHIVertexInputBinding> Bindings;
};

class RHIInputLayout : public RHIResource
{
};

struct RHIShaderDesc
{
	ERHIShaderStage Stage = ERHIShaderStage::Num;
	ERHIShaderLanguage Language = ERHIShaderLanguage::HLSL;

	const class ShaderBinary* Binary = nullptr;
	FName Name;

	inline RHIShaderDesc& SetStage(ERHIShaderStage InStage) { Stage = InStage; return *this; }
	inline RHIShaderDesc& SetLanguage(ERHIShaderLanguage InLanguage) { Language = InLanguage; return *this; }
	inline RHIShaderDesc& SetShaderBinary(const class ShaderBinary* const InBinary) { Binary = InBinary; return *this; }
	inline RHIShaderDesc& SetName(FName&& InName) { Name = std::move(InName); return *this; }
};

class RHIShader
{
public:
	RHIShader(const RHIShaderDesc& Desc)
		: m_Stage(Desc.Stage)
	{
	}

	ERHIShaderStage GetStage() const { return m_Stage; }
private:
	ERHIShaderStage m_Stage = ERHIShaderStage::Num;
};
