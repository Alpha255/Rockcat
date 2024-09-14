#include "ImGuiEditor.h"
#include "Core/Main.h"
#include "Core/Window.h"
#include "DX12Impl.h"
#include "Submodules/ImGuiAl/fonts/CodingFontTobi.inl"
#include "Submodules/ImGuiAl/fonts/RobotoRegular.inl"
#include "Submodules/ImGuiAl/fonts/MaterialDesign.inl"
#include "Runtime/Editor/Icons/IconsMaterialDesignIcons.h"
#include "Editor/Panels/LogConsole.h"
#include "Editor/ImGuiTheme.h"

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
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    //io.ConfigDockingTransparentPayload = true;

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

    for (auto& Entry : std::filesystem::directory_iterator("EditorThemes\\"))
    {
        auto Path = Entry.path().string();
        m_Themes.insert(std::make_pair(Entry.path().stem().string(), ImGuiTheme::Load(Path)));
    }
    SetTheme("Cosy");

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
    //io.Fonts->AddFontDefault();

    io.Fonts->TexGlyphPadding = 1;
    for (int n = 0; n < io.Fonts->ConfigData.Size; n++)
    {
        ImFontConfig* font_config = (ImFontConfig*)&io.Fonts->ConfigData[n];
        font_config->RasterizerMultiply = 1.0f;
    }
    auto Result = io.Fonts->Build();
}

void ImGuiEditor::SetTheme(const std::string& ThemeName)
{
    auto Theme = m_Themes.find(ThemeName);
    if (Theme != m_Themes.end())
    {
        m_Theme = ThemeName;

        ImGuiStyle& Style = ImGui::GetStyle();
        Style = static_cast<ImGuiStyle&>(*Theme->second);
    }
}

void ImGuiEditor::DrawMenuBar()
{
    auto Font = SetFont("CodingFontTobi");

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
            for (auto& [Name, Theme] : m_Themes)
            {
                if (ImGui::MenuItem(Name.c_str(), nullptr, m_Theme == Name))
                {
                    SetTheme(Name);
                }
            }

            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Demo"))
        {
            ImGui::ShowDemoWindow();
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
