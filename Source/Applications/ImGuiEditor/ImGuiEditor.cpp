#include "ImGuiEditor.h"
#include "Core/Main.h"
#include "Core/Window.h"
#include "DX12Impl.h"
#include "Submodules/ImGuiAl/fonts/CodingFontTobi.inl"
#include "Submodules/ImGuiAl/fonts/RobotoRegular.inl"
#include "Submodules/ImGuiAl/fonts/MaterialDesign.inl"
#include "Runtime/Editor/Icons/IconsMaterialDesignIcons.h"
#include "Editor/Panels/LogConsole.h"

class Canvas
{
};

#define IM_VK_KEYPAD_ENTER      (VK_RETURN + 256)
static ImGuiKey ImGui_ImplWin32_VirtualKeyToImGuiKey(WPARAM wParam)
{
    switch (wParam)
    {
    case VK_TAB: return ImGuiKey_Tab;
    case VK_LEFT: return ImGuiKey_LeftArrow;
    case VK_RIGHT: return ImGuiKey_RightArrow;
    case VK_UP: return ImGuiKey_UpArrow;
    case VK_DOWN: return ImGuiKey_DownArrow;
    case VK_PRIOR: return ImGuiKey_PageUp;
    case VK_NEXT: return ImGuiKey_PageDown;
    case VK_HOME: return ImGuiKey_Home;
    case VK_END: return ImGuiKey_End;
    case VK_INSERT: return ImGuiKey_Insert;
    case VK_DELETE: return ImGuiKey_Delete;
    case VK_BACK: return ImGuiKey_Backspace;
    case VK_SPACE: return ImGuiKey_Space;
    case VK_RETURN: return ImGuiKey_Enter;
    case VK_ESCAPE: return ImGuiKey_Escape;
    case VK_OEM_7: return ImGuiKey_Apostrophe;
    case VK_OEM_COMMA: return ImGuiKey_Comma;
    case VK_OEM_MINUS: return ImGuiKey_Minus;
    case VK_OEM_PERIOD: return ImGuiKey_Period;
    case VK_OEM_2: return ImGuiKey_Slash;
    case VK_OEM_1: return ImGuiKey_Semicolon;
    case VK_OEM_PLUS: return ImGuiKey_Equal;
    case VK_OEM_4: return ImGuiKey_LeftBracket;
    case VK_OEM_5: return ImGuiKey_Backslash;
    case VK_OEM_6: return ImGuiKey_RightBracket;
    case VK_OEM_3: return ImGuiKey_GraveAccent;
    case VK_CAPITAL: return ImGuiKey_CapsLock;
    case VK_SCROLL: return ImGuiKey_ScrollLock;
    case VK_NUMLOCK: return ImGuiKey_NumLock;
    case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
    case VK_PAUSE: return ImGuiKey_Pause;
    case VK_NUMPAD0: return ImGuiKey_Keypad0;
    case VK_NUMPAD1: return ImGuiKey_Keypad1;
    case VK_NUMPAD2: return ImGuiKey_Keypad2;
    case VK_NUMPAD3: return ImGuiKey_Keypad3;
    case VK_NUMPAD4: return ImGuiKey_Keypad4;
    case VK_NUMPAD5: return ImGuiKey_Keypad5;
    case VK_NUMPAD6: return ImGuiKey_Keypad6;
    case VK_NUMPAD7: return ImGuiKey_Keypad7;
    case VK_NUMPAD8: return ImGuiKey_Keypad8;
    case VK_NUMPAD9: return ImGuiKey_Keypad9;
    case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
    case VK_DIVIDE: return ImGuiKey_KeypadDivide;
    case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
    case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
    case VK_ADD: return ImGuiKey_KeypadAdd;
    case IM_VK_KEYPAD_ENTER: return ImGuiKey_KeypadEnter;
    case VK_LSHIFT: return ImGuiKey_LeftShift;
    case VK_LCONTROL: return ImGuiKey_LeftCtrl;
    case VK_LMENU: return ImGuiKey_LeftAlt;
    case VK_LWIN: return ImGuiKey_LeftSuper;
    case VK_RSHIFT: return ImGuiKey_RightShift;
    case VK_RCONTROL: return ImGuiKey_RightCtrl;
    case VK_RMENU: return ImGuiKey_RightAlt;
    case VK_RWIN: return ImGuiKey_RightSuper;
    case VK_APPS: return ImGuiKey_Menu;
    case '0': return ImGuiKey_0;
    case '1': return ImGuiKey_1;
    case '2': return ImGuiKey_2;
    case '3': return ImGuiKey_3;
    case '4': return ImGuiKey_4;
    case '5': return ImGuiKey_5;
    case '6': return ImGuiKey_6;
    case '7': return ImGuiKey_7;
    case '8': return ImGuiKey_8;
    case '9': return ImGuiKey_9;
    case 'A': return ImGuiKey_A;
    case 'B': return ImGuiKey_B;
    case 'C': return ImGuiKey_C;
    case 'D': return ImGuiKey_D;
    case 'E': return ImGuiKey_E;
    case 'F': return ImGuiKey_F;
    case 'G': return ImGuiKey_G;
    case 'H': return ImGuiKey_H;
    case 'I': return ImGuiKey_I;
    case 'J': return ImGuiKey_J;
    case 'K': return ImGuiKey_K;
    case 'L': return ImGuiKey_L;
    case 'M': return ImGuiKey_M;
    case 'N': return ImGuiKey_N;
    case 'O': return ImGuiKey_O;
    case 'P': return ImGuiKey_P;
    case 'Q': return ImGuiKey_Q;
    case 'R': return ImGuiKey_R;
    case 'S': return ImGuiKey_S;
    case 'T': return ImGuiKey_T;
    case 'U': return ImGuiKey_U;
    case 'V': return ImGuiKey_V;
    case 'W': return ImGuiKey_W;
    case 'X': return ImGuiKey_X;
    case 'Y': return ImGuiKey_Y;
    case 'Z': return ImGuiKey_Z;
    case VK_F1: return ImGuiKey_F1;
    case VK_F2: return ImGuiKey_F2;
    case VK_F3: return ImGuiKey_F3;
    case VK_F4: return ImGuiKey_F4;
    case VK_F5: return ImGuiKey_F5;
    case VK_F6: return ImGuiKey_F6;
    case VK_F7: return ImGuiKey_F7;
    case VK_F8: return ImGuiKey_F8;
    case VK_F9: return ImGuiKey_F9;
    case VK_F10: return ImGuiKey_F10;
    case VK_F11: return ImGuiKey_F11;
    case VK_F12: return ImGuiKey_F12;
    case VK_F13: return ImGuiKey_F13;
    case VK_F14: return ImGuiKey_F14;
    case VK_F15: return ImGuiKey_F15;
    case VK_F16: return ImGuiKey_F16;
    case VK_F17: return ImGuiKey_F17;
    case VK_F18: return ImGuiKey_F18;
    case VK_F19: return ImGuiKey_F19;
    case VK_F20: return ImGuiKey_F20;
    case VK_F21: return ImGuiKey_F21;
    case VK_F22: return ImGuiKey_F22;
    case VK_F23: return ImGuiKey_F23;
    case VK_F24: return ImGuiKey_F24;
    case VK_BROWSER_BACK: return ImGuiKey_AppBack;
    case VK_BROWSER_FORWARD: return ImGuiKey_AppForward;
    default: return ImGuiKey_None;
    }
}

void ImGuiEditor::OnInitialize()
{
	auto WindowHandle = const_cast<void*>(GetAssociatedWindow()->GetHandle());
	VERIFY(CreateDeviceD3D(reinterpret_cast<::HWND>(WindowHandle)));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigDockingTransparentPayload = true;

    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = ImGui::GetStyle().Colors;
    style.FrameBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    // style.AntiAliasedFill = false;
    // style.WindowRounding = 0.0f;
    style.TabRounding = 3.0f;
    // style.ChildRounding = 0.0f;
    style.PopupRounding = 3.0f;
    // style.FrameRounding = 0.0f;
    // style.ScrollbarRounding = 5.0f;
    Math::Vector2 m_defaultFramePadding(8.0f, 2.0f);
    Math::Vector2 m_defaultWindowPadding(8.0f, 8.0f);
    style.FramePadding = ImVec2(m_defaultFramePadding.x, m_defaultFramePadding.y);
    style.WindowPadding = ImVec2(m_defaultWindowPadding.x, m_defaultWindowPadding.y);
    style.CellPadding = ImVec2(9, 2);
    // style.ItemInnerSpacing = ImVec2(8, 4);
    // style.ItemInnerSpacing = ImVec2(5, 4);
    // style.GrabRounding = 6.0f;
    // style.GrabMinSize     = 6.0f;
    style.ChildBorderSize = 0.0f;
    // style.TabBorderSize = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMenuButtonPosition = ImGuiDir_None;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    //colors[ImGuiCol_PopupBorder] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.04f, 0.04f, 0.04f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.26f, 0.26f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.49f, 0.62f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.37f, 0.53f, 1.00f);
    //colors[ImGuiCol_ButtonLocked] = ImVec4(0.183f, 0.273f, 0.364f, 1.000f);
    //colors[ImGuiCol_ButtonSecondary] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    //colors[ImGuiCol_ButtonSecondaryHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    //colors[ImGuiCol_ButtonSecondaryActive] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    //colors[ImGuiCol_ButtonSecondaryLocked] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    //colors[ImGuiCol_Folder] = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
    //colors[ImGuiCol_FolderHovered] = ImVec4(0.35f, 0.49f, 0.62f, 1.00f);
    //colors[ImGuiCol_FolderActive] = ImVec4(0.24f, 0.37f, 0.53f, 1.00f);
    //colors[ImGuiCol_Toolbar] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
    //colors[ImGuiCol_Icon] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    //colors[ImGuiCol_TitleHeader] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    //colors[ImGuiCol_TitleHeaderHover] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    //colors[ImGuiCol_TitleHeaderPressed] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
    //colors[ImGuiCol_TitleHeaderBorder] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    //colors[ImGuiCol_TitleHeaderDisabled] = ImVec4(0.17f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.43f, 0.24f, 0.24f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.49f, 0.32f, 0.32f, 1.00f);
    colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.24f, 0.25f, 0.26f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.47f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.69f, 0.15f, 0.29f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.47f, 0.20f, 0.20f, 0.71f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.58f, 0.23f, 0.23f, 0.71f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.61f);

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(WindowHandle);
    ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
        DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
        g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
        g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());

    ExtraInitialize();

    m_Panels.emplace_back(std::make_shared<LogConsole>());
}

void ImGuiEditor::ExtraInitialize()
{
    auto Window = GetAssociatedWindow();
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(Window->GetWidth()), static_cast<float>(Window->GetHeight()));
    io.ConfigWindowsMoveFromTitleBarOnly = true;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();

    style.WindowPadding = ImVec2(5, 5);
    style.FramePadding = ImVec2(4, 4);
    style.ItemSpacing = ImVec2(6, 2);
    style.ItemInnerSpacing = ImVec2(2, 2);
    style.IndentSpacing = 6.0f;
    style.TouchExtraPadding = ImVec2(4, 4);

    style.ScrollbarSize = 10;

    style.WindowBorderSize = 0;
    style.ChildBorderSize = 1;
    style.PopupBorderSize = 3;
    style.FrameBorderSize = 0.0f;

    const int roundingAmount = 2;
    style.PopupRounding = roundingAmount;
    style.WindowRounding = roundingAmount;
    style.ChildRounding = 0;
    style.FrameRounding = roundingAmount;
    style.ScrollbarRounding = roundingAmount;
    style.GrabRounding = roundingAmount;
    style.WindowMinSize = ImVec2(200.0f, 200.0f);
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

#if 1
    style.TabBorderSize = 1.0f;
    style.TabRounding = roundingAmount; // + 4;

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = roundingAmount;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
#endif

    ImFontConfig icons_config;

    static const ImWchar ranges[] = {
        0x0020,
        0x00FF,
        0x0400,
        0x044F,
        0,
    };

    io.FontGlobalScale = 1.0f;
    const float FontSize = 13.0f;

    icons_config.MergeMode = false;
    icons_config.PixelSnapH = true;
    icons_config.OversampleH = icons_config.OversampleV = 1;
    icons_config.GlyphMinAdvanceX = 4.0f;
    icons_config.SizePixels = 12.0f;
    //icons_config.FontDataOwnedByAtlas = false;
    m_Fonts["RobotoRegular"] = io.Fonts->AddFontFromMemoryCompressedTTF(RobotoRegular_compressed_data, RobotoRegular_compressed_size, FontSize, &icons_config, ranges);
    m_Fonts["CodingFontTobi"] = io.Fonts->AddFontFromMemoryCompressedTTF(CodingFontTobi_compressed_data, CodingFontTobi_compressed_size, FontSize, &icons_config, ranges);

    static const ImWchar icons_ranges[] = { ICON_MIN_MDI, ICON_MAX_MDI, 0 };
    //ImFontConfig icons_config;
    // merge in icons from Font Awesome
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphOffset.y = 1.0f;
    icons_config.OversampleH = icons_config.OversampleV = 1;
    icons_config.GlyphMinAdvanceX = 4.0f;
    icons_config.SizePixels = 12.0f;
    m_Fonts["MaterialDesign"] = io.Fonts->AddFontFromMemoryCompressedTTF(MaterialDesign_compressed_data, MaterialDesign_compressed_size, FontSize, &icons_config, icons_ranges);

    for (const auto& File : std::filesystem::recursive_directory_iterator("Fonts\\"))
    {
        auto FilePath = File.path();
        auto Extension = FilePath.extension();
        if (Extension == ".ttf" || Extension == ".otf")
        {
            auto FileName = FilePath.stem().string();
            m_Fonts[FileName] = io.Fonts->AddFontFromFileTTF(FilePath.string().c_str(), FontSize, &icons_config, io.Fonts->GetGlyphRangesDefault());
        }
    }
    io.Fonts->AddFontDefault();

    io.Fonts->TexGlyphPadding = 1;
    for (int n = 0; n < io.Fonts->ConfigData.Size; n++)
    {
        ImFontConfig* font_config = (ImFontConfig*)&io.Fonts->ConfigData[n];
        font_config->RasterizerMultiply = 1.0f;
    }
    auto Result = io.Fonts->Build();

    SetTheme(EImGuiTheme::Black);
}

void ImGuiEditor::SetTheme(EImGuiTheme Theme)
{
    static const float max = 255.0f;

    auto& style = ImGui::GetStyle();
    ImVec4* colours = style.Colors;
    SelectedColour = ImVec4(0.28f, 0.56f, 0.9f, 1.0f);

    // colours[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    // colours[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);

    if (Theme == Black)
    {
        ImGui::StyleColorsDark();
        colours[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colours[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colours[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
        colours[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colours[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
        colours[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
        colours[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colours[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colours[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colours[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colours[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
        colours[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colours[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colours[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colours[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colours[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colours[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colours[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
        colours[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colours[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colours[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
        colours[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
        colours[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colours[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colours[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colours[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colours[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colours[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colours[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colours[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
        colours[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colours[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
        colours[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colours[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colours[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colours[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colours[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
        colours[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colours[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        colours[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        colours[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }
    else if (Theme == Dark)
    {
        ImGui::StyleColorsDark();
        ImVec4 Titlebar = ImVec4(40.0f / max, 42.0f / max, 54.0f / max, 1.0f);
        ImVec4 TabActive = ImVec4(52.0f / max, 54.0f / max, 64.0f / max, 1.0f);
        ImVec4 TabUnactive = ImVec4(35.0f / max, 43.0f / max, 59.0f / max, 1.0f);

        SelectedColour = ImVec4(155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f);
        colours[ImGuiCol_Text] = ImVec4(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.00f);
        colours[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);

        IconColour = colours[ImGuiCol_Text];
        colours[ImGuiCol_WindowBg] = TabActive;
        colours[ImGuiCol_ChildBg] = TabActive;

        colours[ImGuiCol_PopupBg] = ImVec4(42.0f / 255.0f, 38.0f / 255.0f, 47.0f / 255.0f, 1.00f);
        colours[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colours[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colours[ImGuiCol_FrameBg] = ImVec4(65.0f / 255.0f, 79.0f / 255.0f, 92.0f / 255.0f, 1.00f);
        colours[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        colours[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);

        colours[ImGuiCol_TitleBg] = Titlebar;
        colours[ImGuiCol_TitleBgActive] = Titlebar;
        colours[ImGuiCol_TitleBgCollapsed] = Titlebar;
        colours[ImGuiCol_MenuBarBg] = Titlebar;

        colours[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.6f, 0.6f, 0.6f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);

        colours[ImGuiCol_CheckMark] = ImVec4(155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f);
        colours[ImGuiCol_SliderGrab] = ImVec4(155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f);
        colours[ImGuiCol_SliderGrabActive] = ImVec4(185.0f / 255.0f, 160.0f / 255.0f, 237.0f / 255.0f, 1.00f);
        colours[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colours[ImGuiCol_ButtonHovered] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f) + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
        colours[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f) + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);

        colours[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colours[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colours[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);

        colours[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colours[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

        colours[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colours[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colours[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colours[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colours[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colours[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colours[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colours[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        colours[ImGuiCol_Header] = TabActive + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
        colours[ImGuiCol_HeaderHovered] = TabActive + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
        colours[ImGuiCol_HeaderActive] = TabActive + ImVec4(0.05f, 0.05f, 0.05f, 0.1f);

#if 1

        colours[ImGuiCol_Tab] = TabUnactive;
        colours[ImGuiCol_TabHovered] = TabActive + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
        colours[ImGuiCol_TabActive] = TabActive;
        colours[ImGuiCol_TabUnfocused] = TabUnactive;
        colours[ImGuiCol_TabUnfocusedActive] = TabActive;
        colours[ImGuiCol_DockingEmptyBg] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colours[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);

#endif
    }
    else if (Theme == Grey)
    {
        ImGui::StyleColorsDark();
        colours[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colours[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        IconColour = colours[ImGuiCol_Text];

        colours[ImGuiCol_ChildBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colours[ImGuiCol_WindowBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colours[ImGuiCol_PopupBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colours[ImGuiCol_Border] = ImVec4(0.12f, 0.12f, 0.12f, 0.71f);
        colours[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
        colours[ImGuiCol_FrameBg] = ImVec4(0.42f, 0.42f, 0.42f, 0.54f);
        colours[ImGuiCol_FrameBgHovered] = ImVec4(0.42f, 0.42f, 0.42f, 0.40f);
        colours[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.67f);
        colours[ImGuiCol_TitleBg] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
        colours[ImGuiCol_TitleBgActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
        colours[ImGuiCol_TitleBgCollapsed] = ImVec4(0.17f, 0.17f, 0.17f, 0.90f);
        colours[ImGuiCol_MenuBarBg] = ImVec4(0.335f, 0.335f, 0.335f, 1.000f);
        colours[ImGuiCol_ScrollbarBg] = ImVec4(0.24f, 0.24f, 0.24f, 0.53f);
        colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
        colours[ImGuiCol_CheckMark] = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
        colours[ImGuiCol_SliderGrab] = ImVec4(0.52f, 0.52f, 0.52f, 1.00f);
        colours[ImGuiCol_SliderGrabActive] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
        colours[ImGuiCol_Button] = ImVec4(0.54f, 0.54f, 0.54f, 0.35f);
        colours[ImGuiCol_ButtonHovered] = ImVec4(0.52f, 0.52f, 0.52f, 0.59f);
        colours[ImGuiCol_ButtonActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
        colours[ImGuiCol_Header] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colours[ImGuiCol_HeaderHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
        colours[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 0.77f);
        colours[ImGuiCol_Separator] = ImVec4(0.000f, 0.000f, 0.000f, 0.137f);
        colours[ImGuiCol_SeparatorHovered] = ImVec4(0.700f, 0.671f, 0.600f, 0.290f);
        colours[ImGuiCol_SeparatorActive] = ImVec4(0.702f, 0.671f, 0.600f, 0.674f);
        colours[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colours[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colours[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colours[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colours[ImGuiCol_TextSelectedBg] = ImVec4(0.73f, 0.73f, 0.73f, 0.35f);
        colours[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        colours[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colours[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colours[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colours[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

#ifdef IMGUI_HAS_DOCK
        colours[ImGuiCol_DockingEmptyBg] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
        colours[ImGuiCol_Tab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colours[ImGuiCol_TabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
        colours[ImGuiCol_TabActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colours[ImGuiCol_TabUnfocused] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colours[ImGuiCol_TabUnfocusedActive] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colours[ImGuiCol_DockingPreview] = ImVec4(0.85f, 0.85f, 0.85f, 0.28f);
#endif
    }
    else if (Theme == Cosy)
    {
        ImVec4 background = ImVec4(39.0f / 255.0f, 40.0f / 255.0f, 47.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 foreground = ImVec4(42.0f / 255.0f, 44.0f / 255.0f, 54.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 text = ImVec4(230.0f / 255.0f, 175.0f / 255.0f, 137.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 text_secondary = ImVec4(159.0f / 255.0f, 159.0f / 255.0f, 176.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 text_background = ImVec4(97.0f / 255.0f, 97.0f / 255.0f, 106.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 text_blue = ImVec4(110.0f / 255.0f, 150.0f / 255.0f, 200.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 text_orange = ImVec4(183.0f / 255.0f, 113.0f / 255.0f, 96.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 text_yellow = ImVec4(214.0f / 255.0f, 199.0f / 255.0f, 130.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 text_red = ImVec4(206.0f / 255.0f, 120.0f / 255.0f, 105.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 highlight_primary = ImVec4(47.0f / 255.0f, 179.0f / 255.0f, 135.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 hover_primary = ImVec4(76.0f / 255.0f, 148.0f / 255.0f, 123.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 highlight_secondary = ImVec4(76.0f / 255.0f, 48.0f / 255.0f, 67.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 hover_secondary = ImVec4(105.0f / 255.0f, 50.0f / 255.0f, 68.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 checkerboard_primary = ImVec4(150.0f / 255.0f, 150.0f / 255.0f, 150.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 checkerboard_secondary = ImVec4(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 255.0f / 255.0f);
        ImVec4 modal_dim = ImVec4(0, 0, 0, 48.0f / 255.0f);

        ImGui::StyleColorsDark();
        colours[ImGuiCol_Text] = text;
        colours[ImGuiCol_TextDisabled] = text_background;
        colours[ImGuiCol_TextSelectedBg] = text_secondary;

        colours[ImGuiCol_WindowBg] = background;
        colours[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colours[ImGuiCol_PopupBg] = background;

        colours[ImGuiCol_Border] = foreground;
        colours[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
        colours[ImGuiCol_FrameBg] = foreground;
        colours[ImGuiCol_FrameBgHovered] = foreground;
        colours[ImGuiCol_FrameBgActive] = foreground;

        colours[ImGuiCol_TitleBg] = foreground;
        colours[ImGuiCol_TitleBgActive] = foreground;
        colours[ImGuiCol_TitleBgCollapsed] = foreground;
        colours[ImGuiCol_MenuBarBg] = foreground;

        colours[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
        colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
        colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colours[ImGuiCol_CheckMark] = checkerboard_primary;
        colours[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
        colours[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
        colours[ImGuiCol_Button] = foreground;
        colours[ImGuiCol_ButtonHovered] = hover_secondary;
        colours[ImGuiCol_ButtonActive] = highlight_secondary;
        colours[ImGuiCol_Header] = highlight_secondary;
        colours[ImGuiCol_HeaderHovered] = highlight_secondary;
        colours[ImGuiCol_HeaderActive] = highlight_secondary;
        colours[ImGuiCol_Separator] = foreground;
        colours[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
        colours[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
        colours[ImGuiCol_ResizeGrip] = highlight_primary;
        colours[ImGuiCol_ResizeGripHovered] = highlight_secondary;
        colours[ImGuiCol_ResizeGripActive] = highlight_secondary;
        colours[ImGuiCol_Tab] = background;
        colours[ImGuiCol_TabHovered] = foreground;
        colours[ImGuiCol_TabActive] = foreground;
        colours[ImGuiCol_TabUnfocused] = background;
        colours[ImGuiCol_TabUnfocusedActive] = foreground;
        colours[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colours[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colours[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
        colours[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
        colours[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
        colours[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
        colours[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
        colours[ImGuiCol_ModalWindowDimBg] = modal_dim;

        IconColour = text_secondary;
    }
    else if (Theme == Light)
    {
        ImGui::StyleColorsLight();
        colours[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        colours[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
        IconColour = colours[ImGuiCol_Text];

        colours[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
        colours[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        colours[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
        colours[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
        colours[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        colours[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colours[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colours[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
        colours[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
        colours[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        colours[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
        colours[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
        colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
        colours[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colours[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
        colours[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colours[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
        colours[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colours[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colours[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
        colours[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colours[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colours[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
        colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colours[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colours[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colours[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colours[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    }
    else if (Theme == Cherry)
    {
        ImGui::StyleColorsDark();
#define HI(v) ImVec4(0.502f, 0.075f, 0.256f, v)
#define MED(v) ImVec4(0.455f, 0.198f, 0.301f, v)
#define LOW(v) ImVec4(0.232f, 0.201f, 0.271f, v)
#define BG(v) ImVec4(0.200f, 0.220f, 0.270f, v)
#define TEXTCol(v) ImVec4(0.860f, 0.930f, 0.890f, v)

        colours[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colours[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
        IconColour = colours[ImGuiCol_Text];

        colours[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
        colours[ImGuiCol_PopupBg] = BG(0.9f);
        colours[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
        colours[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colours[ImGuiCol_FrameBg] = BG(1.00f);
        colours[ImGuiCol_FrameBgHovered] = MED(0.78f);
        colours[ImGuiCol_FrameBgActive] = MED(1.00f);
        colours[ImGuiCol_TitleBg] = LOW(1.00f);
        colours[ImGuiCol_TitleBgActive] = HI(1.00f);
        colours[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
        colours[ImGuiCol_MenuBarBg] = BG(0.47f);
        colours[ImGuiCol_ScrollbarBg] = BG(1.00f);
        colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
        colours[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
        colours[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        colours[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        colours[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        colours[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        colours[ImGuiCol_ButtonHovered] = MED(0.86f);
        colours[ImGuiCol_ButtonActive] = MED(1.00f);
        colours[ImGuiCol_Header] = MED(0.76f);
        colours[ImGuiCol_HeaderHovered] = MED(0.86f);
        colours[ImGuiCol_HeaderActive] = HI(1.00f);
        colours[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
        colours[ImGuiCol_ResizeGripHovered] = MED(0.78f);
        colours[ImGuiCol_ResizeGripActive] = MED(1.00f);
        colours[ImGuiCol_PlotLines] = TEXTCol(0.63f);
        colours[ImGuiCol_PlotLinesHovered] = MED(1.00f);
        colours[ImGuiCol_PlotHistogram] = TEXTCol(0.63f);
        colours[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
        colours[ImGuiCol_TextSelectedBg] = MED(0.43f);
        colours[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
        colours[ImGuiCol_TabHovered] = colours[ImGuiCol_ButtonHovered];
    }
    else if (Theme == Blue)
    {
        ImVec4 colour_for_text = ImVec4(236.f / 255.f, 240.f / 255.f, 241.f / 255.f, 1.0f);
        ImVec4 colour_for_head = ImVec4(41.f / 255.f, 128.f / 255.f, 185.f / 255.f, 1.0f);
        ImVec4 colour_for_area = ImVec4(57.f / 255.f, 79.f / 255.f, 105.f / 255.f, 1.0f);
        ImVec4 colour_for_body = ImVec4(44.f / 255.f, 62.f / 255.f, 80.f / 255.f, 1.0f);
        ImVec4 colour_for_pops = ImVec4(33.f / 255.f, 46.f / 255.f, 60.f / 255.f, 1.0f);
        colours[ImGuiCol_Text] = ImVec4(colour_for_text.x, colour_for_text.y, colour_for_text.z, 1.00f);
        colours[ImGuiCol_TextDisabled] = ImVec4(colour_for_text.x, colour_for_text.y, colour_for_text.z, 0.58f);
        IconColour = colours[ImGuiCol_Text];

        colours[ImGuiCol_WindowBg] = ImVec4(colour_for_body.x, colour_for_body.y, colour_for_body.z, 0.95f);
        colours[ImGuiCol_Border] = ImVec4(colour_for_body.x, colour_for_body.y, colour_for_body.z, 0.00f);
        colours[ImGuiCol_BorderShadow] = ImVec4(colour_for_body.x, colour_for_body.y, colour_for_body.z, 0.00f);
        colours[ImGuiCol_FrameBg] = ImVec4(colour_for_area.x, colour_for_area.y, colour_for_area.z, 1.00f);
        colours[ImGuiCol_FrameBgHovered] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.78f);
        colours[ImGuiCol_FrameBgActive] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 1.00f);
        colours[ImGuiCol_TitleBg] = ImVec4(colour_for_area.x, colour_for_area.y, colour_for_area.z, 1.00f);
        colours[ImGuiCol_TitleBgCollapsed] = ImVec4(colour_for_area.x, colour_for_area.y, colour_for_area.z, 0.75f);
        colours[ImGuiCol_TitleBgActive] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 1.00f);
        colours[ImGuiCol_MenuBarBg] = ImVec4(colour_for_area.x, colour_for_area.y, colour_for_area.z, 0.47f);
        colours[ImGuiCol_ScrollbarBg] = ImVec4(colour_for_area.x, colour_for_area.y, colour_for_area.z, 1.00f);
        colours[ImGuiCol_ScrollbarGrab] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.21f);
        colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.78f);
        colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 1.00f);
        colours[ImGuiCol_CheckMark] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.80f);
        colours[ImGuiCol_SliderGrab] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.50f);
        colours[ImGuiCol_SliderGrabActive] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 1.00f);
        colours[ImGuiCol_Button] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.50f);
        colours[ImGuiCol_ButtonHovered] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.86f);
        colours[ImGuiCol_ButtonActive] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 1.00f);
        colours[ImGuiCol_Header] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.76f);
        colours[ImGuiCol_HeaderHovered] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.86f);
        colours[ImGuiCol_HeaderActive] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 1.00f);
        colours[ImGuiCol_ResizeGrip] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.15f);
        colours[ImGuiCol_ResizeGripHovered] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.78f);
        colours[ImGuiCol_ResizeGripActive] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 1.00f);
        colours[ImGuiCol_PlotLines] = ImVec4(colour_for_text.x, colour_for_text.y, colour_for_text.z, 0.63f);
        colours[ImGuiCol_PlotLinesHovered] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 1.00f);
        colours[ImGuiCol_PlotHistogram] = ImVec4(colour_for_text.x, colour_for_text.y, colour_for_text.z, 0.63f);
        colours[ImGuiCol_PlotHistogramHovered] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 1.00f);
        colours[ImGuiCol_TextSelectedBg] = ImVec4(colour_for_head.x, colour_for_head.y, colour_for_head.z, 0.43f);
        colours[ImGuiCol_PopupBg] = ImVec4(colour_for_pops.x, colour_for_pops.y, colour_for_pops.z, 0.92f);
    }
    else if (Theme == Classic)
    {
        ImGui::StyleColorsClassic();
        IconColour = colours[ImGuiCol_Text];
    }
    else if (Theme == ClassicDark)
    {
        ImGui::StyleColorsDark();
        IconColour = colours[ImGuiCol_Text];
    }
    else if (Theme == ClassicLight)
    {
        ImGui::StyleColorsLight();
        IconColour = colours[ImGuiCol_Text];
    }
    else if (Theme == Cinder)
    {
        colours[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colours[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
        IconColour = colours[ImGuiCol_Text];
        colours[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
        colours[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
        colours[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colours[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colours[ImGuiCol_FrameBgHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
        colours[ImGuiCol_FrameBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colours[ImGuiCol_TitleBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
        colours[ImGuiCol_TitleBgCollapsed] = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
        colours[ImGuiCol_TitleBgActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colours[ImGuiCol_MenuBarBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
        colours[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
        colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
        colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colours[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
        colours[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        colours[ImGuiCol_SliderGrabActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colours[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
        colours[ImGuiCol_ButtonHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
        colours[ImGuiCol_ButtonActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colours[ImGuiCol_Header] = ImVec4(0.92f, 0.18f, 0.29f, 0.76f);
        colours[ImGuiCol_HeaderHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.86f);
        colours[ImGuiCol_HeaderActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colours[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
        colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.92f, 0.18f, 0.29f, 0.78f);
        colours[ImGuiCol_ResizeGripActive] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colours[ImGuiCol_PlotLines] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
        colours[ImGuiCol_PlotLinesHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colours[ImGuiCol_PlotHistogram] = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
        colours[ImGuiCol_PlotHistogramHovered] = ImVec4(0.92f, 0.18f, 0.29f, 1.00f);
        colours[ImGuiCol_TextSelectedBg] = ImVec4(0.92f, 0.18f, 0.29f, 0.43f);
        colours[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    }
    else if (Theme == Dracula)
    {
        ImGui::StyleColorsDark();

        ImVec4 Titlebar = ImVec4(33.0f / max, 34.0f / max, 44.0f / max, 1.0f);
        ImVec4 TabActive = ImVec4(40.0f / max, 42.0f / max, 54.0f / max, 1.0f);
        ImVec4 TabUnactive = ImVec4(35.0f / max, 43.0f / max, 59.0f / max, 1.0f);

        IconColour = ImVec4(183.0f / 255.0f, 158.0f / 255.0f, 220.0f / 255.0f, 1.00f);
        SelectedColour = ImVec4(145.0f / 255.0f, 111.0f / 255.0f, 186.0f / 255.0f, 1.00f);
        colours[ImGuiCol_Text] = ImVec4(244.0f / 255.0f, 244.0f / 255.0f, 244.0f / 255.0f, 1.00f);
        colours[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);

        colours[ImGuiCol_WindowBg] = TabActive;
        colours[ImGuiCol_ChildBg] = TabActive;

        colours[ImGuiCol_PopupBg] = ImVec4(42.0f / 255.0f, 38.0f / 255.0f, 47.0f / 255.0f, 1.00f);
        colours[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colours[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colours[ImGuiCol_FrameBg] = ImVec4(65.0f / 255.0f, 79.0f / 255.0f, 92.0f / 255.0f, 1.00f);
        colours[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        colours[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);

        colours[ImGuiCol_TitleBg] = Titlebar;
        colours[ImGuiCol_TitleBgActive] = Titlebar;
        colours[ImGuiCol_TitleBgCollapsed] = Titlebar;
        colours[ImGuiCol_MenuBarBg] = Titlebar;

        colours[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        colours[ImGuiCol_ScrollbarGrab] = ImVec4(0.6f, 0.6f, 0.6f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.7f, 0.7f, 0.7f, 1.00f);
        colours[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.8f, 0.8f, 0.8f, 1.00f);

        colours[ImGuiCol_CheckMark] = ImVec4(155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f);
        colours[ImGuiCol_SliderGrab] = ImVec4(155.0f / 255.0f, 130.0f / 255.0f, 207.0f / 255.0f, 1.00f);
        colours[ImGuiCol_SliderGrabActive] = ImVec4(185.0f / 255.0f, 160.0f / 255.0f, 237.0f / 255.0f, 1.00f);
        colours[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colours[ImGuiCol_ButtonHovered] = ImVec4(59.0f / 255.0f, 46.0f / 255.0f, 80.0f / 255.0f, 1.0f);
        colours[ImGuiCol_ButtonActive] = colours[ImGuiCol_ButtonHovered] + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);

        colours[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colours[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colours[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);

        colours[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colours[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colours[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);

        colours[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colours[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colours[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colours[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colours[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colours[ImGuiCol_DragDropTarget] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colours[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colours[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colours[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colours[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

        colours[ImGuiCol_Header] = TabActive + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
        colours[ImGuiCol_HeaderHovered] = TabActive + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
        colours[ImGuiCol_HeaderActive] = TabActive + ImVec4(0.05f, 0.05f, 0.05f, 0.1f);

#if 1

        colours[ImGuiCol_Tab] = TabUnactive;
        colours[ImGuiCol_TabHovered] = TabActive + ImVec4(0.1f, 0.1f, 0.1f, 0.1f);
        colours[ImGuiCol_TabActive] = TabActive;
        colours[ImGuiCol_TabUnfocused] = TabUnactive;
        colours[ImGuiCol_TabUnfocusedActive] = TabActive;
        colours[ImGuiCol_DockingEmptyBg] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colours[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);

#endif
    }

    colours[ImGuiCol_Separator] = colours[ImGuiCol_TitleBg];
    colours[ImGuiCol_SeparatorActive] = colours[ImGuiCol_Separator];
    colours[ImGuiCol_SeparatorHovered] = colours[ImGuiCol_Separator];

    colours[ImGuiCol_TabUnfocusedActive] = colours[ImGuiCol_WindowBg];
    colours[ImGuiCol_TabActive] = colours[ImGuiCol_WindowBg];
    colours[ImGuiCol_ChildBg] = colours[ImGuiCol_TabActive];
    colours[ImGuiCol_ScrollbarBg] = colours[ImGuiCol_TabActive];
    colours[ImGuiCol_TableHeaderBg] = colours[ImGuiCol_TabActive];

    colours[ImGuiCol_TitleBgActive] = colours[ImGuiCol_TitleBg];
    colours[ImGuiCol_TitleBgCollapsed] = colours[ImGuiCol_TitleBg];
    colours[ImGuiCol_MenuBarBg] = colours[ImGuiCol_TitleBg];
    colours[ImGuiCol_PopupBg] = colours[ImGuiCol_WindowBg] + ImVec4(0.05f, 0.05f, 0.05f, 0.0f);

    colours[ImGuiCol_Tab] = colours[ImGuiCol_MenuBarBg];
    colours[ImGuiCol_TabUnfocused] = colours[ImGuiCol_MenuBarBg];

    colours[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 0.00f);
    colours[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colours[ImGuiCol_TableBorderLight] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colours[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
}

void ImGuiEditor::DrawMenuBar()
{
    auto Font = SetFont("STXINWEI");

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

            ImGui::Separator();

            if (ImGui::MenuItem("Exit"))
            {
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("About"))
        {
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void ImGuiEditor::OnRenderGUI(Canvas&)
{
    DrawMenuBar();

    for (auto& Panel : m_Panels)
    {
        Panel->OnRender();
    }
}

void ImGuiEditor::OnRenderFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static bool show_demo_window = true;

    //ImGui::ShowDemoWindow(&show_demo_window);

    Canvas GCanvas;
    OnRenderGUI(GCanvas);

    ImGui::Render();

    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();

    FrameContext* frameCtx = WaitForNextFrameResources();
    UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
    frameCtx->CommandAllocator->Reset();

    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    barrier.Transition.pResource = g_mainRenderTargetResource[backBufferIdx];
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    g_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
    g_pd3dCommandList->ResourceBarrier(1, &barrier);

    static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Render Dear ImGui graphics
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
    g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
    g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    g_pd3dCommandList->ResourceBarrier(1, &barrier);
    g_pd3dCommandList->Close();

    g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

    g_pSwapChain->Present(1, 0); // Present with vsync
    //g_pSwapChain->Present(0, 0); // Present without vsync

    UINT64 fenceValue = g_fenceLastSignaledValue + 1;
    g_pd3dCommandQueue->Signal(g_fence, fenceValue);
    g_fenceLastSignaledValue = fenceValue;
    frameCtx->FenceValue = fenceValue;
}

void ImGuiEditor::OnMouseEvent(const MouseEvent& Mouse)
{
    if (ImGui::GetCurrentContext() != nullptr)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (Mouse.OnMove)
        {
            io.AddMousePosEvent(Mouse.Position.x, Mouse.Position.y);
        }
        if (Mouse.Button == EMouseButton::LButton)
        {
            io.AddMouseButtonEvent(0, Mouse.State == EKeyState::Down);
        }
        else if (Mouse.Button == EMouseButton::RButton)
        {
            io.AddMouseButtonEvent(1, Mouse.State == EKeyState::Down);
        }
        else if (Mouse.Button == EMouseButton::MButton)
        {
            io.AddMouseButtonEvent(2, Mouse.State == EKeyState::Down);
        }

        if (Mouse.WheelDelta != 0)
        {
            io.AddMouseWheelEvent(0.0f, (float)Mouse.WheelDelta / WHEEL_DELTA);
        }
    }
}

void ImGuiEditor::OnKeyboardEvent(const KeyboardEvent& Keyboard)
{
    if (ImGui::GetCurrentContext() != nullptr)
    {
        ImGuiIO& io = ImGui::GetIO();

        io.AddKeyEvent(ImGui_ImplWin32_VirtualKeyToImGuiKey((WPARAM)Keyboard.Key), Keyboard.State == EKeyState::Down);
    }
}

void ImGuiEditor::OnWindowResized(uint32_t Width, uint32_t Height)
{
    if (g_pSwapChain)
    {
        WaitForLastSubmittedFrame();
        CleanupRenderTarget();
        HRESULT result = g_pSwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
        assert(SUCCEEDED(result) && "Failed to resize swapchain.");
        CreateRenderTarget();
    }
}

void ImGuiEditor::OnShutdown()
{
    WaitForLastSubmittedFrame();

    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

	CleanupDeviceD3D();
}

REGISTER_APPLICATION(ImGuiEditor, "ImGuiEditor.json")
