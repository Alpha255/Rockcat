#pragma once

#include "Core/Definitions.h"
#include "Core/Cereal.h"

enum class ERenderingPath : uint8_t
{
	ForwardRendering,  /// Tiled Based/Cluster Based/Stencil Based
	DeferredShading, /// Tiled Based/Cluster Based/Stencil Based
	DeferredLighting,
};

enum class ELightingPolicy : uint8_t
{
	None,
	TiledBased,
	ClusterBased,
	StencilBased
};

enum class EShadowTechnique : uint8_t
{
	None,
	Generic,
	//PCF, DESCRIPTION("Percentage - Closer Filtering")
	//PCSS, DESCRIPTION("Percentage - Closer Soft Shadows")
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
	Num
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
		bool Enable = true;

		bool DepthOfField = false;
		bool Blur = false;
		bool Bloom = false;
		bool LensFlare = false;

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
	bool VSync = false;
	bool FullScreen = false;
	bool SRGBSwapchain = false;
	bool MotionVectors = false;
	bool FrustumCulling = true;
	bool EnableTiledBasedLightCulling = false;
	bool EnableClusteredBasedLightCulling = false;
	bool EnableStencilBasedLightCulling = false;
	bool EnableComputeQueue = true;
	bool EnableTransferQueue = true;
	bool BatchResourceDataTransfer = false;
	bool BatchResourceBarrier = false;
	bool AsyncCommandlistSubmission = false;
	bool InverseDepth = false;
	ERenderingPath RenderingPath = ERenderingPath::ForwardRendering;
	ELightingPolicy LightingPolicy = ELightingPolicy::None;
	EShadowTechnique ShadowTechnique = EShadowTechnique::None;
	EAntiAliasingTechnique AntiAliasingTechnique = EAntiAliasingTechnique::None;
	ERenderHardwareInterface RenderHardwareInterface = ERenderHardwareInterface::Num;
	PostProcessingSettings PostProcessing;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Resolution),
			CEREAL_NVP(VSync),
			CEREAL_NVP(FullScreen),
			CEREAL_NVP(SRGBSwapchain),
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
			CEREAL_NVP(InverseDepth),
			CEREAL_NVP(RenderingPath),
			CEREAL_NVP(LightingPolicy),
			CEREAL_NVP(ShadowTechnique),
			CEREAL_NVP(AntiAliasingTechnique),
			CEREAL_NVP(RenderHardwareInterface),
			CEREAL_NVP(PostProcessing)
		);
	}
};
