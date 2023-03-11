#pragma once

#include "Colorful/IRenderer/IShader.h"
#include "Colorful/IRenderer/IImage.h"

struct MaterialProperty
{
	using ValueType = std::variant<
		float32_t,
		int32_t,
		Vector4,
		Vector3,
		Vector2,
		Color,
		Matrix,
		bool8_t>;

	template<class Type>
	void Set(const Type& V)
	{
		Value = V;
	}

	template<class Type>
	Type Get() const
	{
		return std::get<Type>(Value);
	}

	MaterialProperty(const char8_t* PropertyName)
		: Name(PropertyName)
	{
	}

	ValueType Value;
	std::string Name;
};

struct ShaderPermutationBinding
{
	uint32_t Type = 0;
	std::unordered_map<uint32_t, std::vector<RHI::ShaderMacro>> Bindings;

	template<class Enum>
	void Set(Enum EnumType)
	{
		Type = static_cast<uint32_t>(EnumType);
	}

	const std::vector<RHI::ShaderMacro>& Permutation()
	{
		return Bindings[Type];
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Type),
			CEREAL_NVP(Bindings)
		);
	}
};

class Material : public Serializeable<Material>
{
public:
	static std::shared_ptr<ImageAsset> DefaultImage;

	enum EImageType
	{
		AlbedoOrDiffuse,
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
		EmissionColor,
		Metalness,
		Roughness,
		Occlusion,
		Sheen,
		ClearCoat,
		Transmission,
		Unknown
	};

	enum class EShadingMode
	{
		BlinnPhong,
		PhysicallyBasedRendering,
		Toon,
		Others
	};

	Material(const char8_t* Path);

	Material(const Material& Other)
		: m_ShadingMode(Other.m_ShadingMode)
		, m_Attributes(Other.m_Attributes)
		, m_PermutationBindings(Other.m_PermutationBindings)
		, m_Images(Other.m_Images)
		, m_PipelineShaders(Other.m_PipelineShaders)
		, m_DebugName(Other.m_DebugName)
	{
	}

	~Material()
	{
		Save();
	}

	const char8_t* DebugName() const
	{
		return m_DebugName.c_str();
	}

	void SetDebugName(const char8_t* Name)
	{
		m_DebugName.assign(Name);
	}

	const EShadingMode ShadingMode() const
	{
		return m_ShadingMode;
	}

	const RHI::PipelineShaders& PipelineShaders() const
	{
		return m_PipelineShaders;
	}

	RHI::IShader* Shader(RHI::EShaderStage Stage) const
	{
		return m_PipelineShaders.Get(Stage);
	}

	RHI::IImagePtr Image(EImageType Type) const
	{
		assert(Type < EImageType::Unknown);
		return m_Images[Type] ? m_Images[Type]->Object : DefaultImage->Object;
	}

	void SetShadingMode(EShadingMode Mode);

	void SetShader(RHI::EShaderStage Stage, const char8_t* Path)
	{
		SetShader(Stage, Path, true);
	}

	void SetImage(EImageType Type, const char8_t* Path)
	{
		SetImage(Type, Path, true);
	}

	template<class Enum>
	void SwitchPermutation(RHI::EShaderStage Stage, Enum Type)
	{
		static_assert(std::is_integral_v<std::underlying_type_t<Enum>>, "The type must be enum");
		SetDirty(m_PermutationBindings[Stage]->Type == static_cast<uint32_t>(Type));
		m_PermutationBindings[Stage]->Set(Type);
		m_PipelineShaders.Set(Stage, m_Shaders[Stage]->Object->Get(m_PermutationBindings[Stage]->Permutation()));
	}

#define PROPERTY_SETER_GETER(TYPE, VALUE)      \
	TYPE Get##VALUE() const                    \
	{                                          \
		return m_Attributes.VALUE;             \
	}                                          \
	void Set##VALUE(const TYPE& Value)         \
	{                                          \
		m_Attributes.VALUE = Value;            \
		SetDirty(m_Attributes.VALUE == Value); \
	}

	PROPERTY_SETER_GETER(Color, AlbedoOrDiffuse)
	PROPERTY_SETER_GETER(Color, Ambient)
	PROPERTY_SETER_GETER(Color, Specular)
	PROPERTY_SETER_GETER(Color, Emissive)
	PROPERTY_SETER_GETER(Color, Transparent)
	PROPERTY_SETER_GETER(Color, Reflective)
	PROPERTY_SETER_GETER(float32_t, Metallic)
	PROPERTY_SETER_GETER(float32_t, Roughness)
	PROPERTY_SETER_GETER(float32_t, Opacity)
	PROPERTY_SETER_GETER(float32_t, Shiness)
	PROPERTY_SETER_GETER(float32_t, OcclusionStrength)
	PROPERTY_SETER_GETER(float32_t, SpecularFactor)
	PROPERTY_SETER_GETER(bool8_t, TwoSide)

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Serializeable),
			CEREAL_NVP(m_Attributes),
			CEREAL_NVP(m_Images),
			CEREAL_NVP(m_Shaders),
			CEREAL_NVP(m_DebugName),
			CEREAL_NVP(m_PermutationBindings)
		);
	}

	bool8_t IsSameWith(const Material* Other) const
	{
		if (Other)
		{
			for (auto ShaderStage = RHI::EShaderStage::Vertex; ShaderStage < RHI::EShaderStage::ShaderStageCount; ShaderStage = static_cast<RHI::EShaderStage>(ShaderStage + 1))
			{
				if (m_PipelineShaders.Get(ShaderStage) != Other->m_PipelineShaders.Get(ShaderStage))
				{
					return false;
				}
			}

			return true;
		}

		return false;
	}
protected:
	struct MaterialAttributes
	{
		Color AlbedoOrDiffuse{ Color::White };
		Color Ambient{ Color::White };
		Color Specular{ Color::Black };
		Color Emissive{ Color::Black };
		Color Transparent{ Color::White };
		Color Reflective{ Color::Black };
		float32_t Metallic = 0.0f;
		float32_t Roughness = 0.0f;
		float32_t Opacity = 1.0f;
		float32_t Shiness = 1.0f;
		float32_t OcclusionStrength = 1.0f;
		float32_t SpecularFactor = 1.0f;
		bool8_t TwoSide = false;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(AlbedoOrDiffuse),
				CEREAL_NVP(Ambient),
				CEREAL_NVP(Specular),
				CEREAL_NVP(Emissive),
				CEREAL_NVP(Transparent),
				CEREAL_NVP(Reflective),
				CEREAL_NVP(Metallic),
				CEREAL_NVP(Roughness),
				CEREAL_NVP(Opacity),
				CEREAL_NVP(Shiness),
				CEREAL_NVP(OcclusionStrength),
				CEREAL_NVP(SpecularFactor),
				CEREAL_NVP(TwoSide)
			);
		}
	};

	friend class MaterialImporter;

	void OnLoadCompletion() override final;

	void SetShader(RHI::EShaderStage Stage, const char8_t* Path, bool8_t MarkDirty);

	void SetImage(EImageType, const char8_t* Path, bool8_t MarkDirty);
private:
	EShadingMode m_ShadingMode = EShadingMode::BlinnPhong;
	MaterialAttributes m_Attributes;
	std::array<std::shared_ptr<ImageAsset>, EImageType::Unknown> m_Images;
	std::array<std::shared_ptr<ShaderAsset>, RHI::EShaderStage::ShaderStageCount> m_Shaders;
	std::array<std::shared_ptr<ShaderPermutationBinding>, RHI::EShaderStage::ShaderStageCount> m_PermutationBindings;
	std::string m_DebugName;
	RHI::PipelineShaders m_PipelineShaders;
};