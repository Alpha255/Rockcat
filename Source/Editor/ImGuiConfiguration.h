#pragma once

#include "Editor/ImGuiTheme.h"
#include "Core/Math/Color.h"

struct ImGuiFontConfigs : public ImFontConfig
{
	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(Name),
			CEREAL_NVP(FontDataSize),
			CEREAL_NVP(FontDataOwnedByAtlas),
			CEREAL_NVP(MergeMode),
			CEREAL_NVP(PixelSnapH),
			CEREAL_NVP(PixelSnapV),
			CEREAL_NVP(OversampleH),
			CEREAL_NVP(OversampleV),
			CEREAL_NVP(EllipsisChar),
			CEREAL_NVP(SizePixels),
			CEREAL_NVP(GlyphOffset),
			CEREAL_NVP(GlyphMinAdvanceX),
			CEREAL_NVP(GlyphMaxAdvanceX),
			CEREAL_NVP(GlyphExtraAdvanceX),
			CEREAL_NVP(FontNo),
			CEREAL_NVP(FontLoaderFlags),
			CEREAL_NVP(RasterizerMultiply),
			CEREAL_NVP(RasterizerDensity),
			CEREAL_NVP(Flags),
			CEREAL_NVP(GlyphRangesPayload),
			CEREAL_NVP(MergeIconFont),
			CEREAL_NVP(FontSize)
		);

		if constexpr (Archive::is_loading::value)
		{
			GlyphRanges = GlyphRangesPayload.data();
		}
	}

	bool MergeIconFont = false;
	float FontSize = 12.0f;
	std::vector<ImWchar> GlyphRangesPayload;
};

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

struct ImGuiScopedStyle
{
	ImGuiScopedStyle(ImGuiStyleVar_ Var, float Value)
	{
		ImGui::PushStyleVar(Var, Value);
	}

	ImGuiScopedStyle(ImGuiStyleVar_ Var, const Math::Vector2& Value)
	{
		ImGui::PushStyleVar(Var, ImVec2(Value.x, Value.y));
	}

	~ImGuiScopedStyle()
	{
		ImGui::PopStyleVar();
	}
};
#define IMGUI_SCOPED_STYLE(Var, Value) ImGuiScopedStyle CAT(_ScopedStyle_, __LINE__)(Var, Value)

struct ImGuiScopedColor
{
	ImGuiScopedColor(ImGuiCol_ Var, const Math::Color& Col, bool Conditional = true)
		: Condition(Conditional)
	{
		if (Condition)
		{
			ImGui::PushStyleColor(Var, ImVec4(Col.x, Col.y, Col.z, Col.w));
		}
	}

	~ImGuiScopedColor()
	{
		if (Condition)
		{
			ImGui::PopStyleColor();
		}
	}

	bool Condition;
};
#define IMGUI_SCOPED_COLOR(Var, Color) ImGuiScopedColor CAT(_ScopedColor_, __LINE__)(Var, Color)
#define IMGUI_SCOPED_COLOR_CONDITION(Var, Color, Condition) ImGuiScopedColor CAT(_ScopedColor_, __LINE__)(Var, Color, Condition)

struct ImGuiScopedFont
{
	ImGuiScopedFont(const char* const FontName)
	{
		std::string TempFontName(FontName ? FontName : "");
		if (!TempFontName.empty())
		{
			auto FontIt = AllFonts->find(TempFontName);
			if (FontIt != AllFonts->end())
			{
				ImGui::PushFont(FontIt->second.Font);
				FontPushed = true;
			}
		}
	}

	~ImGuiScopedFont()
	{
		if (FontPushed)
		{
			ImGui::PopFont();
		}
	}
private:
	friend class ImGuiConfiguration;
	static const std::map<std::string, ImGuiFont>* AllFonts;
	bool FontPushed = false;
};
#define IMGUI_SCOPED_FONT(FontName) ImGuiScopedFont CAT(_ScopedFont_, __LINE__)(FontName)

struct ImGuiScopedID
{
	explicit ImGuiScopedID(const char* ID) 
	{
		ImGui::PushID(ID);
	}

	explicit ImGuiScopedID(const void* ID)
	{
		ImGui::PushID(ID);
	}

	explicit ImGuiScopedID(int32_t ID)
	{
		ImGui::PushID(ID);
	}

	~ImGuiScopedID()
	{
		ImGui::PopID();
	}
};
#define IMGUI_SCOPED_ID(ID) ImGuiScopedID CAT(_ScopedID_, __LINE__)(ID)

class ImGuiConfiguration : public JsonAsset<ImGuiConfiguration>
{
public:
	using BaseClass::BaseClass;

	~ImGuiConfiguration();

	void SetTheme(const std::string& ThemeName, bool Force = false);
	void SetTheme(const char* const ThemeName, bool Force = false);
	ImGuiTheme* GetTheme() const { return m_Theme; }

	/// Device pixels per logical pixel the editor is drawn at. Everything is authored at 1.0 and
	/// stretched from there, so the UI keeps the same physical size on every monitor.
	///
	/// The glyphs are rasterized again for the new density - scaling the atlas of another one would
	/// leave the text blurry - which means the GPU side has to be rebuilt afterwards, see
	/// VulkanImGuiRenderer::RebuildFontAtlas(). The texture itself is *not* touched here: this class
	/// knows nothing about any backend.
	float GetDisplayScale() const { return m_DisplayScale; }
	bool SetDisplayScale(float DisplayScale);

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_BASE(BaseClass),
			CEREAL_NVP(m_DefaultTheme),
			CEREAL_NVP(m_DefaultFont),
			CEREAL_NVP(m_Fonts)
		);
	}
protected:
	void OnPostLoad() override final;
private:
	friend class ImGuiEditor;

	std::unordered_map<std::string, std::shared_ptr<ImGuiTheme>>& GetThemes() { return m_Themes; }

	/// Re-assigns the selected theme and stretches it to the current density afterwards. The style
	/// object is replaced wholesale by the assignment, so this has to run after every one of them,
	/// including theme switches at runtime.
	void ApplyTheme();

	ImGuiTheme* m_Theme = nullptr;
	std::string m_DefaultTheme;
	std::string m_DefaultFont;

	/// Device pixels per logical pixel. Themes and font sizes coming out of the asset are authored
	/// at 1.0 and only multiplied by this when they are handed to ImGui, so switching densities
	/// rebuilds from the same source instead of compounding onto the previous scale.
	float m_DisplayScale = 1.0f;

	std::unordered_map<std::string, std::shared_ptr<ImGuiTheme>> m_Themes;
	std::map<std::string, ImGuiFont> m_Fonts;

	void LoadDefaultFonts();
	void LoadDefaultThemes();

	friend class ThemeEditorPanel;
};