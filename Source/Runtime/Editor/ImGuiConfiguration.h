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
			CEREAL_NVP(GlyphRangesPayload)
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

class ImGuiConfiguration : public Serializable<ImGuiConfiguration>
{
public:
	using BaseClass::BaseClass;

	~ImGuiConfiguration();

	void SetTheme(const std::string& ThemeName, bool Force = false);
	void SetTheme(const char* const ThemeName, bool Force = false);
	ImGuiTheme* GetTheme() const { return m_Theme; }

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
	void SetStatusChangeCallbacks() override;
private:
	friend class ImGuiEditor;

	std::unordered_map<std::string, std::shared_ptr<ImGuiTheme>>& GetThemes() { return m_Themes; }

	ImGuiTheme* m_Theme = nullptr;
	std::string m_DefaultTheme;
	std::string m_DefaultFont;

	std::unordered_map<std::string, std::shared_ptr<ImGuiTheme>> m_Themes;
	std::map<std::string, ImGuiFont> m_Fonts;

	void LoadDefaultFonts();
	void LoadDefaultThemes();

	friend class ThemeEditorPanel;
};