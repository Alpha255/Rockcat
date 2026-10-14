#include "Editor/ImGuiConfiguration.h"
#include "Core/SpdLogging.h"
#include "Editor/Icons/Icons.h"
#include "Paths.h"

#include <cmath>

const std::map<std::string, ImGuiFont>* ImGuiScopedFont::AllFonts = nullptr;

namespace
{
	/// Size of every font that has no size recorded in the asset, in logical units.
	static constexpr float kDefaultFontSize = 12.0f;

	/// Below this a change of density is not worth rebuilding the atlas for.
	static constexpr float kMinimumScaleStep = 0.01f;
}

void ImGuiConfiguration::OnPostLoad()
{
	LoadDefaultFonts();
	LoadDefaultThemes();
}

void ImGuiConfiguration::LoadDefaultFonts()
{
	auto& IO = ImGui::GetIO();
	IO.FontGlobalScale = 1.0f;

	/// An atlas cannot grow once it is built, and it has to be built again whenever the density
	/// changes: rasterizing at the size the glyphs will actually be blitted at is what keeps them
	/// crisp, where FontGlobalScale would only blow up a bitmap made for another monitor.
	IO.Fonts->Clear();
	IO.FontDefault = nullptr;

	auto AddFont = [this, &IO](const std::filesystem::path& FontPath) -> bool {
		assert(std::filesystem::exists(FontPath));

		const float DefaultFontSize = kDefaultFontSize * m_DisplayScale;
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
			/// Configs is the source size straight out of the asset and has to stay that way, with
			/// the density multiplied on the way out only - otherwise a second rebuild would scale
			/// an already scaled size again. SizePixels wins over the size argument next to it, so
			/// it gets the same treatment; GlyphRanges points into the payload of that very entry,
			/// which outlives the call, hence copying is safe.
			const ImGuiFontConfigs& SourceConfigs = FontIt->second.Configs;
			const float SourceSize = SourceConfigs.SizePixels > 0.0f ? SourceConfigs.SizePixels : SourceConfigs.FontSize;
			const float ScaledFontSize = SourceSize * m_DisplayScale;

			ImGuiFontConfigs ScaledConfigs = SourceConfigs;
			ScaledConfigs.SizePixels = ScaledFontSize;

			FontIt->second.Font = IO.Fonts->AddFontFromFileTTF(FontPath.string().c_str(),
				ScaledFontSize,
				&ScaledConfigs,
				ScaledConfigs.GlyphRanges);
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
		LOG_ERROR(LogDefault, "Failed to build ImGui font atlas!");
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
#if 0
	{
		ImGui::StyleColorsDark();
		auto& Style = ImGui::GetStyle();
		ImGuiTheme Dark(Paths::EditorThemePath() / "Dark.json", Style);
		Dark.Save(true);
	}

	{
		ImGui::StyleColorsLight();
		auto& Style = ImGui::GetStyle();
		ImGuiTheme Light(Paths::EditorThemePath() / "Light.json", Style);
		Light.Save(true);
	}

	{
		ImGui::StyleColorsClassic();
		auto& Style = ImGui::GetStyle();
		ImGuiTheme Classic(Paths::EditorThemePath() / "Classic.json", Style);
		Classic.Save(true);
	}
#endif

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
			LOG_ERROR(LogDefault, "Duplicated ImGui editor theme in different directory!");
		}
	}
	
	SetTheme(m_DefaultTheme, true);
}

void ImGuiConfiguration::SetTheme(const char* const ThemeName, bool Force)
{
	SetTheme(std::string(ThemeName ? ThemeName : ""), Force);
}

void ImGuiConfiguration::ApplyTheme()
{
	if (m_Theme)
	{
		/// Themes carry paddings, radii and indents authored in logical units, so they have to be
		/// stretched once the whole UI moves to another density.
		ImGui::GetStyle() = static_cast<const ImGuiStyle&>(*m_Theme);
		ImGui::GetStyle().ScaleAllSizes(m_DisplayScale);
	}
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
			ApplyTheme();
		}
	}
}

bool ImGuiConfiguration::SetDisplayScale(float DisplayScale)
{
	if (!ImGui::GetCurrentContext() || DisplayScale <= 0.0f ||
		std::fabs(DisplayScale - m_DisplayScale) < kMinimumScaleStep)
	{
		return false;
	}

	m_DisplayScale = DisplayScale;

	LoadDefaultFonts();
	ApplyTheme();

	return true;
}

ImGuiConfiguration::~ImGuiConfiguration()
{
	Save(true);
}