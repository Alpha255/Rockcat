#pragma once

#include "Core/Window.h"
#include "Engine/Application/GraphicsSettings.h"
#include "Engine/Asset/SerializableAsset.h"

class ApplicationConfigurations : public SerializableAsset<ApplicationConfigurations>
{
public:
	using BaseClass::BaseClass;

	bool IsEnableWindow() const { return m_EnableWindow; }
	bool IsEnableRendering() const { return m_EnableRendering; }
	const WindowCreateInfo& GetWindowCreateInfo() const { return m_WindowCreateInfo; }
	const GraphicsSettings& GetGraphicsSettings() const { return m_GraphicsSettings; }

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_EnableWindow),
			CEREAL_NVP(m_EnableRendering),
			CEREAL_NVP(m_WindowCreateInfo),
			CEREAL_NVP(m_GraphicsSettings)
		);
	}
private:
	bool m_EnableWindow = true;
	bool m_EnableRendering = true;

	WindowCreateInfo m_WindowCreateInfo;
	GraphicsSettings m_GraphicsSettings;
};
