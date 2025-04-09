#pragma once

#include "Core/Window.h"
#include "Engine/Application/GraphicsSettings.h"
#include "Engine/Asset/SerializableAsset.h"

struct ApplicationConfiguration : public SerializableAsset<ApplicationConfiguration>
{
	using BaseClass::BaseClass;

	bool EnableRendering = true;

	WindowCreateInfo WindowDesc;
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
