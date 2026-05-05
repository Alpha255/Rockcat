#pragma once

#include "Window.h"
#include "Rendering/RenderSettings.h"
#include "Asset/SerializableAsset.h"

class ApplicationSettings : public Serializable<ApplicationSettings>
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
