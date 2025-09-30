#pragma once

#include "Window.h"
#include "Rendering/RenderSettings.h"
#include "Asset/SerializableAsset.h"

struct ApplicationSettings : public Serializable<ApplicationSettings>
{
	using BaseClass::BaseClass;

	WindowSettings Window;
	RenderSettings Rendering;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass),
			CEREAL_NVP(Window),
			CEREAL_NVP(Rendering)
		);
	}
};
