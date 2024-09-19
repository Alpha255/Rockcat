#include "Editor/ImGuiConfigurations.h"
#include "Engine/Services/SpdLogService.h"
#include <Submodules/ImGuiAl/fonts/MaterialDesign.inl>

const std::map<std::string, ImGuiFont>* ImGuiScopedFont::AllFonts = nullptr;

void ImGuiConfigurations::PostLoad()
{
	LoadDefaultFonts();
	LoadDefaultThemes();
}

void ImGuiConfigurations::LoadDefaultFonts()
{
	auto& IO = ImGui::GetIO();
	IO.FontGlobalScale = 1.0f;
	const float DefaultFontSize = 14.0f;

	for (auto& Entry : std::filesystem::recursive_directory_iterator("Fonts\\"))
	{
		if (Entry.is_directory())
		{
			continue;
		}

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
				&FontIt->second.Configs,
				FontIt->second.Configs.GlyphRanges);
		}
	}

	if (!IO.Fonts->Build())
	{
		LOG_ERROR("Failed to build ImGui font atlas!");
	}
	else
	{
		ImGuiScopedFont::AllFonts = &m_Fonts;
		if (!m_DefaultFont.empty())
		{
			auto FontIt = m_Fonts.find(m_DefaultFont);
			if (FontIt != m_Fonts.end())
			{
				IO.FontDefault = FontIt->second.Font;
			}
		}
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
			auto Theme = ImGuiTheme::Load(ThemePath);
			Theme->Name = ThemeName;
			m_Themes.insert(std::make_pair(ThemeName, std::move(Theme)));
		}
		else
		{
			LOG_ERROR("Duplicated ImGui editor theme in different directory!");
		}
	}
	
	SetTheme(m_DefaultTheme, true);
}

void ImGuiConfigurations::SetTheme(const char* const ThemeName, bool Force)
{
	SetTheme(std::string(ThemeName ? ThemeName : ""), Force);
}

void ImGuiConfigurations::SetTheme(const std::string& ThemeName, bool Force)
{
	if (!ThemeName.empty() && (m_DefaultTheme != ThemeName || Force))
	{
		auto ThemeIt = m_Themes.find(ThemeName);
		if (ThemeIt != m_Themes.end())
		{
			m_DefaultTheme = ThemeName;
			m_Theme = ThemeIt->second.get();
			ImGui::GetStyle() = static_cast<const ImGuiStyle&>(*m_Theme);
		}
	}
}

ImGuiConfigurations::~ImGuiConfigurations()
{
	Save<ImGuiConfigurations>(true);
}