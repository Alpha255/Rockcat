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
	Default,
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

enum class ERHIDeviceType : uint8_t
{
	Software,
	Vulkan,
	D3D12,
	D3D11,
	OpenGL,
	Num
};

struct RenderSettings
{
	struct DebugDrawSettings
	{
		enum class EGBufferDebugMode : uint8_t
		{
			None,
			BaseColor,
			Normal,
			Roughness,
			Metalness,
			Occlusion,
			Specular,
			Emissive
		};

		bool Wireframe = false;
		EGBufferDebugMode GBufferDebugMode = EGBufferDebugMode::None;

		template<class Archive>
		void serialize(Archive& Ar)
		{
			Ar(
				CEREAL_NVP(Wireframe),
				CEREAL_NVP(GBufferDebugMode)
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

	bool Enable = true;
	bool VSync = false;
	bool FullScreen = false;
	bool HDR = false;
	
	bool MotionVectors = false;
	bool FrustumCulling = true;
	bool EnableTiledBasedLightCulling = false;
	bool EnableClusteredBasedLightCulling = false;

	bool EnableStencilBasedLightCulling = false;
	bool EnableAsyncCommandlistSubmission = false;
	bool EnableAsyncMeshDrawCommandsBuilding = false;
	bool InverseDepth = false;

	ERenderingPath RenderingPath = ERenderingPath::ForwardRendering;
	ELightingPolicy LightingPolicy = ELightingPolicy::Default;
	EShadowTechnique ShadowTechnique = EShadowTechnique::None;
	EAntiAliasingTechnique AntiAliasingTechnique = EAntiAliasingTechnique::None;

	ERHIDeviceType DeviceType = ERHIDeviceType::Num;
	
	PostProcessingSettings PostProcessing;
	DebugDrawSettings DebugDraw;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Enable),
			CEREAL_NVP(VSync),
			CEREAL_NVP(FullScreen),
			CEREAL_NVP(HDR),
			CEREAL_NVP(MotionVectors),
			CEREAL_NVP(FrustumCulling),
			CEREAL_NVP(EnableTiledBasedLightCulling),
			CEREAL_NVP(EnableClusteredBasedLightCulling),
			CEREAL_NVP(EnableStencilBasedLightCulling),
			CEREAL_NVP(EnableAsyncCommandlistSubmission),
			CEREAL_NVP(EnableAsyncMeshDrawCommandsBuilding),
			CEREAL_NVP(InverseDepth),
			CEREAL_NVP(RenderingPath),
			CEREAL_NVP(LightingPolicy),
			CEREAL_NVP(ShadowTechnique),
			CEREAL_NVP(AntiAliasingTechnique),
			CEREAL_NVP(DeviceType),
			CEREAL_NVP(PostProcessing),
			CEREAL_NVP(DebugDraw)
		);
	}
};
