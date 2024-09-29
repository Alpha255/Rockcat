#include "Editor/ImGuiEditor.h"
#include "Editor/Panels/LogConsole.h"
#include "Editor/Panels/GameView.h"
#include "Editor/Panels/AssetBrowser.h"

ImGuiEditor::ImGuiEditor(uint32_t AppWindowWidth, uint32_t AppWindowHeight)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

    ImGuiIO& IO = ImGui::GetIO();
    IO.DisplaySize = ImVec2(static_cast<float>(AppWindowWidth), static_cast<float>(AppWindowHeight));
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    IO.ConfigWindowsMoveFromTitleBarOnly = true;
    //io.ConfigDockingTransparentPayload = true;
	
	m_Configs = ImGuiConfigurations::Load();
    
    m_Panels.emplace_back(std::make_shared<LogConsole>());
    m_Panels.emplace_back(std::make_shared<GameView>());
    m_Panels.emplace_back(std::make_shared<AssetBrowser>());
}

void ImGuiEditor::Draw()
{
    DrawMenuBar();

    for (auto& Panel : m_Panels)
    {
        Panel->Draw();
    }

    if (m_ShowThemeEditor)
    {
        auto CurrentTheme = m_Configs->GetTheme();
        if (ImGui::Begin(CurrentTheme->Name.c_str()))
        {
            ImGui::ShowStyleEditor(CurrentTheme);
            ImGui::End();
        }
    }
}

void ImGuiEditor::DrawMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::BeginMenu("Open"))
            {
                if (ImGui::MenuItem("Open Recent"))
                {
                }
                if (ImGui::MenuItem("Open Last"))
                {
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Save"))
            {
            }

            if (ImGui::MenuItem("Exit"))
            {
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Theme"))
        {
            auto CurrentTheme = m_Configs->GetTheme();

            for (auto& [Name, Theme] : m_Configs->GetThemes())
            {
                if (ImGui::MenuItem(Name.c_str(), nullptr, CurrentTheme && CurrentTheme->Name == Name))
                {
                    m_Configs->SetTheme(Name);
                }
            }

            if (ImGui::MenuItem("ThemeEditor") && CurrentTheme)
            {
                m_ShowThemeEditor = true;
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("About"))
        {
            ImGui::ShowAboutWindow();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}