#pragma once

#include "Runtime/Asset/Asset.h"

NAMESPACE_START(RHI)

enum class ERenderingPath : uint8_t
{
	ForwardShading,  /// Tiled Based/Cluster Based/Stencil Based
	DeferredShading, /// Tiled Based/Cluster Based/Stencil Based
	DeferredLighting,
};

enum class EShadowTechnique : uint8_t
{
	None,
	ShadowMap,
	PCF, DESCRIPTION("Percentage - Closer Filtering")
	PCSS, DESCRIPTION("Percentage - Closer Soft Shadows")
	Cascade
};

enum class EToneMappingTechnique : uint8_t
{
	None,
	Reinhard,
	Uncharted2,
	ACES, DESCRIPTION("Academy Color Encoding System")
};

enum class AntiAliasingTechnique : uint8_t
{
	None,
	MSAA, DESCRIPTION("MultiSampling Anti-Aliasing")
	TAA, DESCRIPTION("Temporal Anti-Aliasing")
	FXAA, DESCRIPTION("Fast Approximate Anti-Aliasing")
};

struct RenderSettings : public Serializeable<RenderSettings>
{
	struct DisplayResolution
	{
		uint32_t Width = 1280u;
		uint32_t Height = 720u;

		uint32_t MinWidth = 640u;
		uint32_t MinHeight = 480u;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(Width),
				CEREAL_NVP(Height),
				CEREAL_NVP(MinWidth),
				CEREAL_NVP(MinHeight)
			);
		}
	};

	struct PostProcessingSettings
	{
		bool8_t Enable = true;

		bool8_t DOF = false;
		bool8_t Blur = false;
		bool8_t Bloom = false;
		bool8_t GodRay = false;

		EToneMappingTechnique ToneMappingTechnique = EToneMappingTechnique::None;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(Enable),
				CEREAL_NVP(DOF),
				CEREAL_NVP(Blur),
				CEREAL_NVP(Bloom),
				CEREAL_NVP(GodRay),
				CEREAL_NVP(ToneMappingTechnique)
			);
		}
	};

	DisplayResolution Resolution;
	bool8_t VSync = false;
	bool8_t FullScreen = false;
	bool8_t MotionVectors = false;
	bool8_t FrustumCulling = true;
	bool8_t EnableTiledBasedLightCulling = false;
	bool8_t EnableClusteredBasedLightCulling = false;
	bool8_t EnableStencilBasedLightCulling = false;
	ERenderingPath RenderingPath = ERenderingPath::ForwardShading;
	EShadowTechnique ShadowTechnique = EShadowTechnique::None;
	PostProcessingSettings PostProcessing;

	RenderSettings(const char8_t* Path)
		: Serializeable(IAsset::CatPath(ASSET_PATH_SETTINGS, Path))
	{
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(Serializeable),
			CEREAL_NVP(Resolution),
			CEREAL_NVP(VSync),
			CEREAL_NVP(FullScreen),
			CEREAL_NVP(MotionVectors),
			CEREAL_NVP(FrustumCulling),
			CEREAL_NVP(EnableTiledBasedLightCulling),
			CEREAL_NVP(EnableClusteredBasedLightCulling),
			CEREAL_NVP(EnableStencilBasedLightCulling),
			CEREAL_NVP(RenderingPath),
			CEREAL_NVP(ShadowTechnique),
			CEREAL_NVP(PostProcessing),
			CEREAL_NVP(Renderer)
		);
	}

	const char8_t* const RendererName() const
	{
		return Renderer.c_str();
	}
protected:
	friend class cereal::access;
	std::string Renderer{"Vulkan"};
};

NAMESPACE_END(RHI)
