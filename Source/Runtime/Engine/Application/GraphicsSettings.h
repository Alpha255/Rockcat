#pragma once

#include "Runtime/Core/Definitions.h"
#include "Runtime/Core/Cereal.h"

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

enum class EAntiAliasingTechnique : uint8_t
{
	None,
	MSAA, DESCRIPTION("MultiSampling Anti-Aliasing")
	TAA, DESCRIPTION("Temporal Anti-Aliasing")
	FXAA, DESCRIPTION("Fast Approximate Anti-Aliasing")
};

enum class ERenderHardwareInterface : uint8_t
{
	Software,
	Vulkan,
	D3D11,
	D3D12,
	Null
};

struct GraphicsSettings
{
	struct DisplayResolution
	{
		uint32_t Width = 1280u;
		uint32_t Height = 720u;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(Width),
				CEREAL_NVP(Height)
			);
		}
	};

	struct PostProcessingSettings
	{
		bool8_t Enable = true;

		bool8_t DepthOfField = false;
		bool8_t Blur = false;
		bool8_t Bloom = false;
		bool8_t LensFlare = false;

		EToneMappingTechnique ToneMappingTechnique = EToneMappingTechnique::None;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(Enable),
				CEREAL_NVP(DepthOfField),
				CEREAL_NVP(Blur),
				CEREAL_NVP(Bloom),
				CEREAL_NVP(LensFlare),
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
	bool8_t EnableComputeQueue = true;
	bool8_t EnableTransferQueue = true;
	bool8_t BatchResourceDataTransfer = false;
	bool8_t BatchResourceBarrier = false;
	bool8_t AsyncCommandlistSubmission = false;
	ERenderingPath RenderingPath = ERenderingPath::ForwardShading;
	EShadowTechnique ShadowTechnique = EShadowTechnique::None;
	EAntiAliasingTechnique AntiAliasingTechnique = EAntiAliasingTechnique::None;
	ERenderHardwareInterface RenderHardwareInterface = ERenderHardwareInterface::Null;
	PostProcessingSettings PostProcessing;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Resolution),
			CEREAL_NVP(VSync),
			CEREAL_NVP(FullScreen),
			CEREAL_NVP(MotionVectors),
			CEREAL_NVP(FrustumCulling),
			CEREAL_NVP(EnableTiledBasedLightCulling),
			CEREAL_NVP(EnableClusteredBasedLightCulling),
			CEREAL_NVP(EnableStencilBasedLightCulling),
			CEREAL_NVP(EnableComputeQueue),
			CEREAL_NVP(EnableTransferQueue),
			CEREAL_NVP(BatchResourceDataTransfer),
			CEREAL_NVP(BatchResourceBarrier),
			CEREAL_NVP(AsyncCommandlistSubmission),
			CEREAL_NVP(RenderingPath),
			CEREAL_NVP(ShadowTechnique),
			CEREAL_NVP(AntiAliasingTechnique),
			CEREAL_NVP(RenderHardwareInterface),
			CEREAL_NVP(PostProcessing)
		);
	}
};
