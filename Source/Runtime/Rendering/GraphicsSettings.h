#pragma once

#include "Core/Cereal.h"

enum class ERenderingPath
{
	Forward,
	TiledBasedForward,
	ClusterBasedForward,
	Deferred,
	TiledBasedDeferred,
	ClusterBasedDeferred,
	GpuDriven
};

enum class ELightingPolicy : uint8_t
{
	Default,
	TiledBased,
	ClusterBased,
	StencilBased
};

enum class EShadowTechnique
{
	None,
	Generic,
	PCF,
	PCSS,
	Cascade
};

enum class EToneMappingTechnique
{
	None,
	Reinhard,
	Uncharted2,
	ACES
};

enum class EAntiAliasingTechnique
{
	None,
	MSAA,
	TAA,
	FXAA
};

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

struct DebugDrawSettings
{
	bool Wireframe = false;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Wireframe)
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
			CEREAL_NVP_ENUM(EToneMappingTechnique, ToneMappingTechnique)
		);
	}
};

struct GraphicsSettings
{
	bool VSync = false;
	bool HDR = false;
	
	bool MotionVectors = false;
	bool FrustumCulling = true;
	bool EnableTiledBasedLightCulling = false;
	bool EnableClusteredBasedLightCulling = false;

	bool EnableStencilBasedLightCulling = false;
	bool EnableAsyncCommandlistSubmission = false;
	bool EnableAsyncMeshDrawCommandsBuilding = false;
	bool InverseDepth = false;

	bool StereoRendering = false;

	ERenderingPath RenderingPath = ERenderingPath::Forward;
	ELightingPolicy LightingPolicy = ELightingPolicy::Default;
	EShadowTechnique ShadowTechnique = EShadowTechnique::None;
	EAntiAliasingTechnique AntiAliasingTechnique = EAntiAliasingTechnique::None;
	
	PostProcessingSettings PostProcessing;
	DebugDrawSettings DebugDraw;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(VSync),
			CEREAL_NVP(HDR),
			CEREAL_NVP(MotionVectors),
			CEREAL_NVP(FrustumCulling),
			CEREAL_NVP(EnableTiledBasedLightCulling),
			CEREAL_NVP(EnableClusteredBasedLightCulling),
			CEREAL_NVP(EnableStencilBasedLightCulling),
			CEREAL_NVP(EnableAsyncCommandlistSubmission),
			CEREAL_NVP(EnableAsyncMeshDrawCommandsBuilding),
			CEREAL_NVP(InverseDepth),
			CEREAL_NVP(StereoRendering),
			CEREAL_NVP_ENUM(ERenderingPath, RenderingPath),
			CEREAL_NVP_ENUM(ELightingPolicy, LightingPolicy),
			CEREAL_NVP_ENUM(EShadowTechnique, ShadowTechnique),
			CEREAL_NVP_ENUM(EAntiAliasingTechnique, AntiAliasingTechnique),
			CEREAL_NVP(PostProcessing),
			CEREAL_NVP(DebugDraw)
		);
	}
};
