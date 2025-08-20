#pragma once

#include "Window.h"
#include "Rendering/RenderSettings.h"
#include "Asset/SerializableAsset.h"

struct ApplicationSettings : public SerializableAsset<ApplicationSettings>
{
	using BaseClass::BaseClass;

	bool EnableRendering = true;

	WindowDesc WindowDesc;
	RenderSettings GraphicsSettings;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(EnableRendering),
			CEREAL_NVP(WindowDesc),
			CEREAL_NVP(GraphicsSettings)
		);
	}
};
