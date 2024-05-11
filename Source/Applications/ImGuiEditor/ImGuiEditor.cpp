#include "ImGuiEditor.h"
#include "Core/Main.h"
#include "Core/Window.h"
#include "DX12Impl.h"

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
}

void ImGuiEditor::OnRenderFrame()
{
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    static bool show_demo_window = true;

    ImGui::ShowDemoWindow(&show_demo_window);

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
