#include "Editor/ImGuiEditor.h"
#include "Editor/Panels/LogConsole.h"
#include "Editor/Panels/SceneView.h"
#include "Editor/Panels/AssetBrowser.h"
#include <Submodules/IconFontCppHeaders/IconsMaterialDesignIcons.h>
//#include <Assets/Fonts/Icons/Blender/IconsBlenderIcons.h>

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
    
    auto LogPanel = m_Panels.emplace_back(std::make_shared<LogConsole>()).get();
    m_Panels.emplace_back(std::make_shared<SceneView>());
    m_Panels.emplace_back(std::make_shared<AssetBrowser>());

    SpdLogService::Get().RegisterRedirector(Cast<LogConsole>(LogPanel));
}

void ImGuiEditor::Draw()
{
    DrawMenuBar();
    
    {
        ScopedDockSpace DockPanels("ImGuiEditorPanels");
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
            if (ImGui::MenuItem(ICON_MDI_FOLDER_OPEN " Open"))
            {
            }

            if (ImGui::BeginMenu("Open Recent", !m_RecentScenes.empty()))
            {
            }

            ImGui::Separator();
            if (ImGui::MenuItem(ICON_MDI_FILE_EDIT_OUTLINE " New", "Ctrl+N"))
            {
            }
            if (ImGui::MenuItem(ICON_MDI_CONTENT_SAVE " Save", "Ctrl+S"))
            {
            }
            if (ImGui::MenuItem(ICON_MDI_RELOAD " Reload", "Ctrl+R"))
            {
            }

            ImGui::Separator();
            if (ImGui::MenuItem(ICON_MDI_CLOSE_CIRCLE " Exit"))
            {
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Settings"))
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
