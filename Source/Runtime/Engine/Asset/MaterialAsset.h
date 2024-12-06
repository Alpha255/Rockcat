#pragma once

#include "Core/Math/Matrix.h"
#include "Core/Math/Color.h"
#include "Engine/Asset/SerializableAsset.h"
#include "Engine/Asset/TextureAsset.h"
#include "Engine/Asset/ShaderAsset.h"
#include "Runtime/Engine/RHI/RHIRenderStates.h"

enum class EShadingMode : uint8_t
{
	Unlit,
	BlinnPhong,
	StandardPBR,
	Toon
};

struct MaterialProperty : public SerializableAsset<MaterialProperty>
{
	using BaseClass::BaseClass;

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
		BaseColor,
		EmissionColor,
		Metalness,
		DiffuseRoughness,
		AmbientOcclusion,
		Num
	};

	std::string Name;

	ERHICullMode CullMode = ERHICullMode::BackFace;
	EShadingMode ShadingMode = EShadingMode::BlinnPhong;
	bool DoubleSided = false;
	bool Wireframe = false;

	float AlphaCutoff = 0.0f;

	struct
	{
		Math::Color BaseColor = Math::Color::White;
		Math::Color DiffuseColor = Math::Color::White;
		Math::Color SpecularColor = Math::Color::White;
		Math::Color EmissiveColor = Math::Color::White;
		Math::Color TransparentColor = Math::Color::White;
		Math::Color ReflectiveColor = Math::Color::White;
		float Metalness = 1.0f;
		float Roughness = 1.0f;
		float Glossiness = 1.0f;
		float Specular = 1.0f;
		float Opacity = 1.0f;
		float Shininess = 1.0f;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(BaseColor),
				CEREAL_NVP(DiffuseColor),
				CEREAL_NVP(SpecularColor),
				CEREAL_NVP(EmissiveColor),
				CEREAL_NVP(TransparentColor),
				CEREAL_NVP(ReflectiveColor),
				CEREAL_NVP(Metalness),
				CEREAL_NVP(Roughness),
				CEREAL_NVP(Glossiness),
				CEREAL_NVP(Specular),
				CEREAL_NVP(Opacity),
				CEREAL_NVP(Shininess)
			);
		}
	} Factors;

	std::array<std::shared_ptr<TextureAsset>, (size_t)ETextureType::Num> Textures;

	inline std::shared_ptr<TextureAsset>& GetTexture(ETextureType Type) 
	{
		assert(Type < ETextureType::Num);
		return Textures[static_cast<size_t>(Type)];
	}

	inline void MarkDirty(bool InDirty) { m_Dirty |= InDirty; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Name),
			CEREAL_NVP(CullMode),
			CEREAL_NVP(ShadingMode),
			CEREAL_NVP(DoubleSided),
			CEREAL_NVP(Wireframe),
			CEREAL_NVP(AlphaCutoff),
			CEREAL_NVP(Factors),
			CEREAL_NVP(Textures)
		);
	}
};

using MaterialID = ObjectID<MaterialProperty, uint32_t>;
