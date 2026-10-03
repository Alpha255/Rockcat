#include "Editor/ImGuiEditor.h"
#include "Editor/Panels/LogConsolePanel.h"
#include "Editor/Panels/SceneViewPanel.h"
#include "Editor/Panels/AssetBrowserPanel.h"
#include "Editor/Icons/Icons.h"
#include "Paths.h"

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
	
	m_Configs = ImGuiConfiguration::Load(Paths::ConfigPath() / "EditorSettings.json");
    
    auto LogPanel = m_Panels.emplace_back(std::make_shared<LogConsolePanel>()).get();
    m_Panels.emplace_back(std::make_shared<SceneViewPanel>());
    m_Panels.emplace_back(std::make_shared<AssetBrowserPanel>());

    SpdLogService::Get().RegisterRedirector(Cast<LogConsolePanel>(LogPanel));
}

void ImGuiEditor::Draw()
{
    DrawMenuBar();
    
    {
        ScopedDockSpace DockPanels("ImGuiEditorPanels");
        IMGUI_SCOPED_STYLE(ImGuiStyleVar_WindowPadding, Math::Vector2(0.0f, 2.0f));
        for (auto& Panel : m_Panels)
        {
            Panel->Draw();
        }
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
            if (ImGui::MenuItemWithStress(ICON_FILE_OPEN " O^pen", "Ctrl+O"))
            {
            }

            if (ImGui::BeginMenuWithStress("Open R^ecent", !m_RecentScenes.empty()))
            {
            }

            ImGui::Separator();
            if (ImGui::MenuItemWithStress(ICON_FILE_NEW " N^ew", "Ctrl+N"))
            {
            }
            if (ImGui::MenuItemWithStress(ICON_FILE_SAVE " S^ave", "Ctrl+S"))
            {
            }
            if (ImGui::MenuItemWithStress(ICON_FILE_RELOAD " R^eload", "Ctrl+R"))
            {
            }

            ImGui::Separator();
            if (ImGui::MenuItemWithStress(ICON_EXIT " Q^uit", "Ctrl+Q"))
            {
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
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

ImGuiEditor::ScopedDockSpace::ScopedDockSpace(const char* const DockSpaceName)
    : Open(true)
    , DockNodeFlags(ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_NoCloseButton)
    , WindowFlags(ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus)
    , Name(DockSpaceName)
{
    if (DockNodeFlags & ImGuiDockNodeFlags_DockSpace)
    {
        WindowFlags |= ImGuiWindowFlags_NoBackground;
    }

    auto Viewport = ImGui::GetMainViewport();
    auto Size = Viewport->Size;
    auto Pos = Viewport->Pos;

    const float FrameHeight = ImGui::GetFrameHeight();
    Pos.y += FrameHeight;
    Size.y -= FrameHeight;

    ImGui::SetNextWindowPos(Pos);
    ImGui::SetNextWindowSize(Size);
    ImGui::SetNextWindowViewport(Viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin(Name, &Open, WindowFlags);
    ImGuiIO& IO = ImGui::GetIO();
    if (IO.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGui::DockSpace(ImGui::GetID(Name), ImVec2(0.0f, 0.0f), DockNodeFlags);
    }
}

ImGuiEditor::ScopedDockSpace::~ScopedDockSpace()
{
    ImGui::PopStyleVar(3);
    ImGui::End();
}
