#pragma once

#include "Engine/Application/BaseApplication.h"
#include <Submodules/imgui/imgui.h>

class ImGuiEditor final : public BaseApplication
{
public:
	using BaseApplication::BaseApplication;
protected:
	void OnInitialize() override final;
	void OnRenderGUI(class Canvas&) override final;
	void OnRenderFrame() override final;
	void OnMouseEvent(const MouseEvent& Mouse) override final;
	void OnKeyboardEvent(const KeyboardEvent& Keyboard) override final;
	void OnWindowResized(uint32_t Width, uint32_t Height) override final;
	void OnShutdown() override final;
private:
	void ExtraInitialize();
	void DrawMenuBar();

    std::unordered_map<std::string, ImFont*> m_Fonts;

    class ScopedFont
    {
    public:
        ScopedFont(const char* const Name, const std::unordered_map<std::string, ImFont*>& Fonts)
        {
            auto FontIt = Fonts.find(Name);
            Font = FontIt == Fonts.end() ? nullptr : (*FontIt).second;
            if (Font)
            {
                ImGui::PushFont(Font);
            }
        }

        ~ScopedFont()
        {
            if (Font)
            {
                ImGui::PopFont();
                Font = nullptr;
            }
        }
    private:
        ImFont* Font;
    };

    std::shared_ptr<ScopedFont> SetFont(const char* const Name) { return std::make_shared<ScopedFont>(Name, m_Fonts); }

    enum EImGuiTheme
    {
        Black = 0,
        Dark,
        Dracula,
        Grey,
        Light,
        Blue,
        ClassicLight,
        ClassicDark,
        Classic,
        Cherry,
        Cinder,
        Cosy
    };

    void SetTheme(EImGuiTheme Theme);
    ImVec4 SelectedColour = ImVec4(0.28f, 0.56f, 0.9f, 1.0f);
    ImVec4 IconColour = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    std::vector<std::shared_ptr<class ImGuiPanel>> m_Panels;
};

static inline ImVec2 operator*(const ImVec2& lhs, const float rhs)
{
    return ImVec2(lhs.x * rhs, lhs.y * rhs);
}
static inline ImVec2 operator/(const ImVec2& lhs, const float rhs)
{
    return ImVec2(lhs.x / rhs, lhs.y / rhs);
}
static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y);
}
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y);
}
static inline ImVec2 operator*(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x * rhs.x, lhs.y * rhs.y);
}
static inline ImVec2 operator/(const ImVec2& lhs, const ImVec2& rhs)
{
    return ImVec2(lhs.x / rhs.x, lhs.y / rhs.y);
}
static inline ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
static inline ImVec2& operator-=(ImVec2& lhs, const ImVec2& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}
static inline ImVec2& operator*=(ImVec2& lhs, const float rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}
static inline ImVec2& operator/=(ImVec2& lhs, const float rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}
static inline ImVec4 operator-(const ImVec4& lhs, const ImVec4& rhs)
{
    return ImVec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
}
static inline ImVec4 operator+(const ImVec4& lhs, const ImVec4& rhs)
{
    return ImVec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
}
static inline ImVec4 operator*(const ImVec4& lhs, const float rhs)
{
    return ImVec4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
}
static inline ImVec4 operator/(const ImVec4& lhs, const float rhs)
{
    return ImVec4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
}
static inline ImVec4 operator*(const ImVec4& lhs, const ImVec4& rhs)
{
    return ImVec4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
}
static inline ImVec4 operator/(const ImVec4& lhs, const ImVec4& rhs)
{
    return ImVec4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w);
}
static inline ImVec4& operator+=(ImVec4& lhs, const ImVec4& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    lhs.z += rhs.z;
    lhs.w += rhs.w;
    return lhs;
}
static inline ImVec4& operator-=(ImVec4& lhs, const ImVec4& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    lhs.z -= rhs.z;
    lhs.w -= rhs.w;
    return lhs;
}
static inline ImVec4& operator*=(ImVec4& lhs, const float rhs)
{
    lhs.x *= rhs;
    lhs.y *= rhs;
    return lhs;
}
static inline ImVec4& operator/=(ImVec4& lhs, const float rhs)
{
    lhs.x /= rhs;
    lhs.y /= rhs;
    return lhs;
}