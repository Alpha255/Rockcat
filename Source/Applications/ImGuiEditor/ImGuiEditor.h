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

    void SetTheme(const std::string& ThemeName);
    ImVec4 SelectedColour = ImVec4(0.28f, 0.56f, 0.9f, 1.0f);
    ImVec4 IconColour = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    std::vector<std::shared_ptr<class ImGuiPanel>> m_Panels;
    std::unordered_map<std::string, std::shared_ptr<struct ImGuiTheme>> m_Themes;
    std::string m_Theme;
};