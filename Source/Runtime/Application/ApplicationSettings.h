#pragma once

#include "Window.h"
#include "Rendering/RenderSettings.h"
#include "Asset/JsonAsset.h"

class ApplicationSettings : public JsonAsset<ApplicationSettings>
{
public:
	using BaseClass::BaseClass;

	inline const WindowSettings& GetWindowSettings() const { return m_WindowSettings; }
	inline const RenderSettings& GetRenderSettings() const { return m_RenderSettings; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass),
			CEREAL_NVP(m_WindowSettings),
			CEREAL_NVP(m_RenderSettings)
		);
	}
private:
	WindowSettings m_WindowSettings;
	RenderSettings m_RenderSettings;
};

enum class EWindowMode
{
	Windowed,
	BorderlessFullscreen,
	ExclusiveFullscreen
};

enum class ERenderBackend
{
	Software,
	Vulkan,
	D3D12,
	D3D11,
	OpenGL,
	Num
};

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

struct DebugDrawSettings
{
	bool Wireframe = false;
};

struct PostProcessingSettings
{
	bool Enable = true;
};

struct GraphicsSettings
{
	bool StereoRendering = false;
	bool VSync = false;
	bool FullScreen = false;
	bool FrustumCulling = true;
	bool InverseDepth = false;

	ERenderingPath RenderingPath = ERenderingPath::Forward;
};

struct ApplicationSettings2
{
	uint32_t WindowWidth = 1280u;
	uint32_t WindowHeight = 720u;
	EWindowMode WindowMode = EWindowMode::Windowed;

	ERenderBackend RenderBackend = ERenderBackend::Vulkan;
};