#pragma once

#include "Runtime/Core/Window.h"
#include "Runtime/Engine/Application/GraphicsSettings.h"
#include "Runtime/Engine/Asset/SerializableAsset.h"

class ApplicationConfigurations : public SerializableAsset<ApplicationConfigurations>
{
public:
	using ParentClass::ParentClass;

	bool8_t IsEnableWindow() const { return m_EnableWindow; }
	bool8_t IsEnableRendering() const { return m_EnableRendering; }
	const WindowCreateInfo& GetWindowCreateInfo() const { return m_WindowCreateInfo; }
	const GraphicsSettings& GetGraphicsSettings() const { return m_GraphicsSettings; }
	GraphicsSettings& GetGraphicsSettings() { return m_GraphicsSettings; }

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
	bool8_t m_EnableWindow = true;
	bool8_t m_EnableRendering = true;

	WindowCreateInfo m_WindowCreateInfo;
	GraphicsSettings m_GraphicsSettings;
};
