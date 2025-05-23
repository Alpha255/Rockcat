#include "Editor/ImGuiConfiguration.h"
#include "Services/SpdLogService.h"
#include "Editor/Icons/Icons.h"
#include "Paths.h"

const std::map<std::string, ImGuiFont>* ImGuiScopedFont::AllFonts = nullptr;

void ImGuiConfiguration::PostLoad()
{
	LoadDefaultFonts();
	LoadDefaultThemes();
}

void ImGuiConfiguration::LoadDefaultFonts()
{
	auto& IO = ImGui::GetIO();
	IO.FontGlobalScale = 1.0f;

	auto AddFont = [this, &IO](const std::filesystem::path& FontPath) -> bool {
		assert(std::filesystem::exists(FontPath));

		const float DefaultFontSize = 12.0f;
		bool MergeWithIconFont = false;
		auto FontName = FontPath.stem().string();
		auto FontIt = m_Fonts.find(FontName);
		if (FontIt == m_Fonts.end())
		{
			auto& FontConfig = m_Fonts.insert(std::make_pair(FontName, ImGuiFont
				{
					ImGuiFontConfigs(),
					IO.Fonts->AddFontFromFileTTF(FontPath.string().c_str(), DefaultFontSize)
				})).first->second.Configs;
			strcpy_s(FontConfig.Name, FontName.c_str());
			MergeWithIconFont = FontConfig.MergeIconFont;
		}
		else
		{
			FontIt->second.Font = IO.Fonts->AddFontFromFileTTF(FontPath.string().c_str(),
				FontIt->second.Configs.FontSize,
				&FontIt->second.Configs,
				FontIt->second.Configs.GlyphRanges);
			MergeWithIconFont = FontIt->second.Configs.MergeIconFont;
		}

		return MergeWithIconFont;
	};

	for (auto& Entry : std::filesystem::directory_iterator(Paths::FontPath()))
	{
		if (Entry.is_directory())
		{
			continue;
		}

		if (AddFont(Entry.path()))
		{
			AddFont(ICON_FONT);
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

void ImGuiConfiguration::LoadDefaultThemes()
{
	for (auto& Entry : std::filesystem::recursive_directory_iterator(Paths::EditorThemePath()))
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

void ImGuiConfiguration::SetTheme(const char* const ThemeName, bool Force)
{
	SetTheme(std::string(ThemeName ? ThemeName : ""), Force);
}

void ImGuiConfiguration::SetTheme(const std::string& ThemeName, bool Force)
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

ImGuiConfiguration::~ImGuiConfiguration()
{
	Save<ImGuiConfiguration>(true);
}