#pragma once

#include "Editor/ImGuiTheme.h"

struct ImGuiFont
{
	ImGuiFontConfigs Configs;
	ImFont* Font = nullptr;

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Configs)
		);
	}
};

class ImGuiConfigurations : public SerializableAsset<ImGuiConfigurations>
{
public:
	ImGuiConfigurations()
		: BaseClass::BaseClass("Configs\\ImGuiEditorConfigs.json")
	{
		LoadDefaultThemes();
		LoadDefaultFonts();
	}

	void SetFont(const char* const FontName);

	void SetTheme(const char* const ThemeName);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_ThemeName),
			CEREAL_NVP(m_Fonts)
		);
	}
private:
	ImGuiTheme* m_Theme = nullptr;
	ImFont* m_Font = nullptr;

	std::string m_ThemeName;
	std::string m_FontName;

	std::unordered_map<std::string, std::shared_ptr<ImGuiTheme>> m_Themes;
	std::unordered_map<std::string, ImGuiFont> m_Fonts;

	void LoadDefaultFonts();
	void LoadDefaultThemes();

	friend class ThemeEditorPanel;
};