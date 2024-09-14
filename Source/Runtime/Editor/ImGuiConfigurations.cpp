#include "Editor/ImGuiConfigurations.h"
#include "Engine/Services/SpdLogService.h"

void ImGuiConfigurations::LoadDefaultFonts()
{
	auto& IO = ImGui::GetIO();
	const float DefaultFontSize = 12.0f;

	for (auto& Entry : std::filesystem::recursive_directory_iterator("Fonts\\"))
	{
		auto FontPath = Entry.path();
		auto FontName = FontPath.stem().string();
		auto FontIt = m_Fonts.find(FontName);
		if (FontIt == m_Fonts.end())
		{
			m_Fonts.insert(std::make_pair(FontName, ImGuiFont{
				ImGuiFontConfigs(),
				IO.Fonts->AddFontFromFileTTF(FontPath.string().c_str(), DefaultFontSize)
			}));
		}
		else
		{
			FontIt->second.Font = IO.Fonts->AddFontFromFileTTF(FontPath.string().c_str(),
				FontIt->second.Configs.FontSize,
				&FontIt->second.Configs);
		}
	}

	if (!IO.Fonts->Build())
	{
		LOG_ERROR("Failed to build ImGui font atlas!");
	}
}

void ImGuiConfigurations::LoadDefaultThemes()
{
	for (auto& Entry : std::filesystem::recursive_directory_iterator("Configs\\EditorThemes"))
	{
		auto ThemePath = Entry.path();
		auto ThemeName = ThemePath.stem().string();
		if (!m_Themes.contains(ThemeName))
		{
			m_Themes.insert(std::make_pair(ThemeName, ImGuiTheme::Load(ThemePath)));
		}
		else
		{
			LOG_ERROR("Duplicated ImGui editor theme in different directory!");
		}
	}
}

void ImGuiConfigurations::SetFont(const char* const FontName)
{
	std::string TempFontName(FontName ? FontName : "");
	if (m_FontName != TempFontName)
	{
		auto FontIt = m_Fonts.find(TempFontName);
		if (FontIt != m_Fonts.end())
		{
			if (m_Font)
			{
				ImGui::PopFont();
			}

			m_FontName.swap(TempFontName);
			m_Font = FontIt->second.Font;

			ImGui::PushFont(m_Font);
		}
	}
}

void ImGuiConfigurations::SetTheme(const char* const ThemeName)
{
	std::string TempThemeName(ThemeName ? ThemeName : "");
	if (m_ThemeName != TempThemeName)
	{
		auto ThemeIt = m_Themes.find(TempThemeName);
		if (ThemeIt != m_Themes.end())
		{
			m_ThemeName.swap(TempThemeName);
			m_Theme = ThemeIt->second.get();
			ImGui::GetStyle() = static_cast<const ImGuiStyle&>(*m_Theme);
		}
	}
}