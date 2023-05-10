#pragma once

#include "Colorful/IRenderer/IResource.h"
#include "Runtime/Asset/Asset.h"

NAMESPACE_START(RHI)

enum class EShaderLanguage
{
	GLSL,
	HLSL
};

enum EShaderStage
{
	Vertex,
	Hull,
	Domain,
	Geometry,
	Fragment,
	Compute,
	ShaderStageCount
};


enum class EVertexInputRate
{
	Vertex,
	Instance,
};

struct InputLayoutDesc : public IHashedResourceDesc
{
	struct VertexAttribute
	{
		uint32_t Location = 0u;
		uint32_t Stride = 0u;

		EFormat Format = EFormat::Unknown;
		std::string Usage;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(Location),
				CEREAL_NVP(Stride),
				CEREAL_NVP(Format),
				CEREAL_NVP(Usage)
			);
		}
	};

	struct VertexInputBinding
	{
		uint32_t Binding = 0u;
		uint32_t Stride = 0u;
		EVertexInputRate InputRate = EVertexInputRate::Vertex;

		std::vector<VertexAttribute> Attributes;

		inline VertexInputBinding& AddAttribute(const VertexAttribute& Attribute)
		{
			Stride += Attribute.Stride;
			Attributes.emplace_back(Attribute);
			return *this;
		}

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(Binding),
				CEREAL_NVP(Stride),
				CEREAL_NVP(InputRate),
				CEREAL_NVP(Attributes)
			);
		}
	};

	std::vector<VertexInputBinding> Bindings;

	size_t Hash() const
	{
		if (HashValue == InvalidHash)
		{
			HashValue = ComputeHash(Bindings.size());
			for (auto& binding : Bindings)
			{
				HashCombine(HashValue, ComputeHash(binding.Binding, binding.Stride, binding.InputRate));

				for (auto& attr : binding.Attributes)
				{
					HashCombine(HashValue, ComputeHash(attr.Location, attr.Stride, attr.Format));
				}
			}
		}

		return HashValue;
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Bindings)
		);
	}
};

class IInputLayout : public IHWResource
{
};

struct ShaderMacro
{
	std::string Name;
	std::string Definition;

	static std::string ToPermutation(const std::vector<ShaderMacro>& Macros);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Name),
			CEREAL_NVP(Definition)
		);
	}
};

struct ShaderVariable
{
public:
	struct Description
	{
		EResourceType Type = EResourceType::Unknown;
		uint32_t Binding = std::numeric_limits<uint8_t>().max();
		EShaderStage Stage = EShaderStage::ShaderStageCount;
		uint32_t ConstantsSize = ~0u;
		std::string Name;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(Name),
				CEREAL_NVP(Binding),
				CEREAL_NVP(Stage),
				CEREAL_NVP(Type),
				CEREAL_NVP(ConstantsSize)
			);
		}
	};

	/// Index | Slot | Stage | Type
	ShaderVariable(uint32_t Index, const Description& Description)
		: ID(
			(Index << static_cast<uint32_t>(EMasks::Index)) |
			(Description.Binding << static_cast<uint32_t>(EMasks::Slot)) |
			(static_cast<uint32_t>(Description.Stage) << static_cast<uint32_t>(EMasks::Stage)) |
			(static_cast<uint32_t>(Description.Type) << static_cast<uint32_t>(EMasks::Type)))
		, DebugName(Description.Name.c_str())
	{
		assert(Index < std::numeric_limits<uint8_t>().max());
		assert(Description.Binding <= std::numeric_limits<uint8_t>().max());
	}

	uint32_t Index() const
	{
		return ID >> static_cast<uint32_t>(EMasks::Index);
	}

	uint32_t Slot() const
	{
		return ID >> static_cast<uint32_t>(EMasks::Slot) & 0x0000000F;
	}

	EShaderStage ShaderStage() const
	{
		return static_cast<EShaderStage>(ID >> static_cast<uint32_t>(EMasks::Stage) & 0x0000000F);
	}

	EResourceType ResourceType() const
	{
		return static_cast<EResourceType>(ID >> static_cast<uint32_t>(EMasks::Type) & 0x0000000F);
	}

	const char8_t* Name() const
	{
		return DebugName.data();
	}

	template<class T>
	T* Cast()
	{
		return static_cast<T*>(Resource.get());
	}

	template<class T>
	const T* Cast() const
	{
		return static_cast<const T*>(Resource.get());
	}

	IHWResource* Get() const
	{
		return HWResource.get();
	}

	void Set(const std::shared_ptr<IHWResource>& Resource)
	{
		HWResource = Resource;
	}
protected:
	enum class EMasks
	{
		Type = 0,
		Stage = 1 << 3,
		Slot = 1 << 4,
		Index = EMasks::Stage + EMasks::Slot,
	};

	uint32_t ID = 0u;
	std::string_view DebugName;
	std::shared_ptr<IHWResource> HWResource;
};

struct ShaderDesc
{
	EShaderStage Stage = EShaderStage::ShaderStageCount;
	EShaderLanguage Language = EShaderLanguage::HLSL;

	size_t BinarySize = 0u;
	std::vector<uint32_t> Binary;

	std::string Name;

	std::vector<ShaderVariable::Description> Variables;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Stage),
			CEREAL_NVP(Language),
			CEREAL_NVP(BinarySize),
			CEREAL_NVP(Binary),
			CEREAL_NVP(Name),
			CEREAL_NVP(Variables)
		);
	}
};

struct ShaderCompiled
{
	IShaderSharedPtr Shader;
	std::shared_ptr<ShaderDesc> Desc;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Desc)
		);
	}
};

using ShaderVariants = std::unordered_map<std::string, ShaderCompiled>;

struct ShaderCache : public Serializeable<ShaderCache>
{
	ShaderVariants Variants;

	ShaderCache(const char8_t* Path)
		: Serializeable(GetCachedPath(Path))
	{
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Variants),
			CEREAL_NVP(LastWriteTime)
		);
	}

	IShaderSharedPtr Get(const std::vector<ShaderMacro>& Macros);
private:
	friend class ShaderCompiler;
	friend class ShaderImporter;

	static std::string GetCachedPath(const char8_t* Path);

	FileTime LastWriteTime;
};

class IShader : IHWResource
{
public:
	IShader(const ShaderDesc& Desc)
		: m_Stage(Desc.Stage)
		, m_Desc(&Desc)
	{
	}

	const ShaderDesc* Desc() const
	{
		return m_Desc;
	}

	EShaderStage Stage() const
	{
		return m_Stage;
	}
protected:
	friend class ShaderImporter;

	EShaderStage m_Stage = EShaderStage::ShaderStageCount;
	const ShaderDesc* m_Desc = nullptr;
};

struct PipelineShaders
{
	IShader* Get(EShaderStage Stage) const
	{
		return Shaders[Stage].get();
	}

	void Set(EShaderStage Stage, const IShaderSharedPtr& Shader)
	{
		assert(Shader->Stage() == Stage);
		Shaders[Stage] = Shader;
	}

	template<class VisitFunc>
	void ForEach(VisitFunc Visit)
	{
		for (auto Stage = EShaderStage::Vertex; Stage < EShaderStage::ShaderStageCount; Stage = static_cast<EShaderStage>(Stage + 1))
		{
			if (auto Shader = Get(Stage))
			{
				Visit(Shader);
			}
		}
	}

	template<class VisitFunc>
	void ForEach(VisitFunc Visit) const
	{
		for (auto Stage = EShaderStage::Vertex; Stage < EShaderStage::ShaderStageCount; Stage = static_cast<EShaderStage>(Stage + 1))
		{
			if (auto Shader = Get(Stage))
			{
				Visit(Shader);
			}
		}
	}

	std::array<IShaderSharedPtr, EShaderStage::ShaderStageCount> Shaders;
};

class PipelineShaderVariableTable
{
public:
	void Set(EShaderStage Stage, uint32_t Slot, const std::shared_ptr<IHWResource>& Resource)
	{
		if (auto Variable = Find(Stage, Slot))
		{
			Variable->Set(Resource);
		}
	}

	void Set(EShaderStage Stage, const char8_t* Name, const std::shared_ptr<IHWResource>& Resource)
	{
		if (auto Variable = Find(Stage, Name))
		{
			Variable->Set(Resource);
		}
	}

	const std::vector<ShaderVariable>& Get() const
	{
		return m_Variables;
	}
protected:
	friend struct PipelineState;

	ShaderVariable* Find(EShaderStage Stage, uint32_t Slot)
	{
		auto Ret = std::find_if(m_Variables.begin(), m_Variables.end(), [Stage, Slot](const ShaderVariable& Variable) {
			return Variable.ShaderStage() == Stage && Variable.Slot() == Slot;
			});
		if (Ret != m_Variables.end())
		{
			return Ret._Unwrapped();
		}

		return nullptr;
	}

	ShaderVariable* Find(EShaderStage Stage, const char8_t* Name)
	{
		auto Ret = std::find_if(m_Variables.begin(), m_Variables.end(), [Stage, Name](const ShaderVariable& Variable) {
			return Variable.ShaderStage() == Stage && std::strcmp(Variable.Name(), Name) == 0;
			});
		if (Ret != m_Variables.end())
		{
			return Ret._Unwrapped();
		}

		return nullptr;
	}
private:
	std::vector<ShaderVariable> m_Variables;
};

#define SHADER_VARIABLE(Name, Type, Register) \
private:                                      \
	class Type* m_##Name;                     \
public:                                       \
	void Set##Name(Type* Resource)            \
	{                                         \
		m_##Name = Resource;                  \
	}                                         \
    Type* Name() const                        \
	{										  \
		return m_##Name;                      \
	}


class IGlobalShader : public IShader
{
public:
	IGlobalShader(const ShaderDesc& Desc)
		: IShader(Desc)
	{
	}
};

NAMESPACE_END(RHI)

using ShaderAsset = Asset<RHI::ShaderCache, IAsset::ECategory::Shader>;
