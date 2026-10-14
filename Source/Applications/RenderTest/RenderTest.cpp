#include "Applications/RenderTest/RenderTest.h"
#include "Application/ApplicationManager.h"
#include "Scene/Scene.h"
#include "Scene/SceneView.h"
#include "Editor/ImGuiEditor.h"
#include "Rendering/RHI/RHIDevice.h"
#include "Rendering/RHI/RHISwapchain.h"
#include "Rendering/RHI/RHICommandListContext.h"
#include "RHI/Vulkan/VulkanSwapchain.h"
#include <imgui/imgui.h>
#include "Core/SpdLogging.h"

#include <chrono>
#include <cmath>
#include <cstdlib>

//#include "Rendering/RenderGraph/RenderGraph.h"

//#include "Asset/GlobalShaders.h"
#include "Scene/Components/TransformComponent.h"

//#include "Asset/ShaderPermutation.h"
#include "Core/Name.h"
#include "Async/Task.h"
#include "Scene/Components/CameraComponent.h"
#include "Scene/Components/StaticMeshComponent.h"
#include "Application/ApplicationSettings.h"

//class TestPemutation1 : public ShaderDefineInt32<0, 1, 2, 3>
//{
//};
//
//class TestPemutation2 : public ShaderDefineBool
//{
//};
//
//class Permutation : public ShaderVariants<TestPemutation1, TestPemutation2>
//{
//};

namespace
{
	/// Validation hook: a single-monitor dev machine cannot change the OS scaling between runs, so
	/// the density the whole pipeline (swapchain, font atlas, style) is driven with can be forced
	/// through the environment. Empty or out-of-range values keep the real monitor DPI.
	float QueryDisplayScale(float RealScale)
	{
		static const float Override = []
		{
			if (const char* Value = std::getenv("RENDERTEST_DPI_SCALE"))
			{
				const float Parsed = std::strtof(Value, nullptr);
				if (Parsed > 0.25f && Parsed <= 8.0f)
				{
					return Parsed;
				}
			}

			return 0.0f;
		}();

		return Override > 0.0f ? Override : RealScale;
	}

	/// EKeyboardKey -> ImGuiKey. EKeyboardKey spells the alphanumeric and function keys with their
	/// (uppercase) ASCII/ordinal code and ImGuiKey lays them out contiguously in the same order, so
	/// those ranges are derived arithmetically and only the named keys need an explicit mapping.
	ImGuiKey ToImGuiKey(EKeyboardKey Key)
	{
		switch (Key)
		{
		case EKeyboardKey::Tab:        return ImGuiKey_Tab;
		case EKeyboardKey::Left:       return ImGuiKey_LeftArrow;
		case EKeyboardKey::Right:      return ImGuiKey_RightArrow;
		case EKeyboardKey::Up:         return ImGuiKey_UpArrow;
		case EKeyboardKey::Down:       return ImGuiKey_DownArrow;
		case EKeyboardKey::PageUp:     return ImGuiKey_PageUp;
		case EKeyboardKey::PageDown:   return ImGuiKey_PageDown;
		case EKeyboardKey::Home:       return ImGuiKey_Home;
		case EKeyboardKey::End:        return ImGuiKey_End;
		case EKeyboardKey::Insert:     return ImGuiKey_Insert;
		case EKeyboardKey::Delete:     return ImGuiKey_Delete;
		case EKeyboardKey::Backspace:  return ImGuiKey_Backspace;
		case EKeyboardKey::Space:      return ImGuiKey_Space;
		case EKeyboardKey::Enter:      return ImGuiKey_Enter;
		case EKeyboardKey::Esc:        return ImGuiKey_Escape;
		case EKeyboardKey::LeftCtrl:   return ImGuiKey_LeftCtrl;
		case EKeyboardKey::LeftShift:  return ImGuiKey_LeftShift;
		case EKeyboardKey::LeftAlt:    return ImGuiKey_LeftAlt;
		case EKeyboardKey::RightCtrl:  return ImGuiKey_RightCtrl;
		case EKeyboardKey::RightShift: return ImGuiKey_RightShift;
		case EKeyboardKey::RightAlt:   return ImGuiKey_RightAlt;
		case EKeyboardKey::LeftMenu:   return ImGuiKey_Menu;
		case EKeyboardKey::RightMenu:  return ImGuiKey_Menu;
		case EKeyboardKey::CapsLock:   return ImGuiKey_CapsLock;
		default: break;
		}

		const auto Code = static_cast<uint8_t>(Key);

		if (Code >= static_cast<uint8_t>(EKeyboardKey::Num0) && Code <= static_cast<uint8_t>(EKeyboardKey::Num9))
		{
			return static_cast<ImGuiKey>(ImGuiKey_0 + (Code - static_cast<uint8_t>(EKeyboardKey::Num0)));
		}

		if (Code >= static_cast<uint8_t>(EKeyboardKey::A) && Code <= static_cast<uint8_t>(EKeyboardKey::Z))
		{
			return static_cast<ImGuiKey>(ImGuiKey_A + (Code - static_cast<uint8_t>(EKeyboardKey::A)));
		}

		if (Code >= static_cast<uint8_t>(EKeyboardKey::F1) && Code <= static_cast<uint8_t>(EKeyboardKey::F12))
		{
			return static_cast<ImGuiKey>(ImGuiKey_F1 + (Code - static_cast<uint8_t>(EKeyboardKey::F1)));
		}

		return ImGuiKey_None;
	}

	/// -1 for the buttons ImGui does not track.
	int ToImGuiMouseButton(EMouseButton Button)
	{
		switch (Button)
		{
		case EMouseButton::Left:   return 0;
		case EMouseButton::Right:  return 1;
		case EMouseButton::Middle: return 2;
		default:                   return -1;
		}
	}
}

void RenderTest::Initialize()
{
	auto TaskA = std::make_shared<TFTask>("TaskA", []() {
		LOG_INFO(LogDefault, "TaskA");
	});

	auto TaskB = TFTask::Launch("TaskB", []() {
		LOG_INFO(LogDefault, "TaskB");
	}, {TaskA.get()});

	auto TaskC = std::make_shared<TFTask>("TaskC", []() {
		LOG_INFO(LogDefault, "TaskC");
	});

	auto TaskD = std::make_shared<TFTask>("TaskD", []() {
		LOG_INFO(LogDefault, "TaskD");
	}, ETFTaskThread::WorkerThread, ETFTaskPriority::High);

	TaskC->AddPrerequisite(*TaskB);
	TaskC->AddPrerequisite(*TaskD);
	TaskC->Trigger();
	TaskD->Trigger();

	//TaskA->Wait();
	//TaskB->Wait();
	//TaskC->Wait();
	//TaskD->Wait();

	auto GetState = [](std::shared_ptr<TFTask>& Task) {
		return Task->IsCompleted() ? "Completed" : "Uncompleted";
	};

	LOG_INFO(LogDefault, "TaskA is {}, TaskB is {}, TaskC is {}, TaskD is {}", GetState(TaskA), GetState(TaskB), GetState(TaskC), GetState(TaskD));

	TaskD->Wait();

	//size_t NumVariants = Permutation::Num;

	auto Comp = new CameraComponent();
	const auto IsACamera = Comp->IsA<CameraComponent>();
	const auto IsABase = Comp->IsA<ComponentBase>();
	const auto IsATransform = Comp->IsA<TransformComponent>();

	auto StaticMesh = new StaticMeshComponent();
	const auto IsAStaticMesh = StaticMesh->IsA<StaticMeshComponent>();
	const auto IsAPrimitive = StaticMesh->IsA<PrimitiveComponent>();
	const auto IsATrans = StaticMesh->IsA<TransformComponent>();
	const auto IsACam = StaticMesh->IsA<CameraComponent>();

	//m_Scene = Scene::Load(Paths::ScenePath() / "RenderTest.json");

	//m_Scene->AddEntity(EntityID::NullIndex, "entity0");
	//m_Scene->AddEntity(EntityID::NullIndex, "entity1");

	//auto Entity0 = m_Scene->GetEntity("entity0");
	//auto Entity1 = m_Scene->GetEntity("entity1");

	//auto Trans0 = Entity0->AddComponent<TransformComponent>();
	//auto Trans1 = Entity1->AddComponent<TransformComponent>();

	//Trans0->SetTranslation(1, 1, 1);
	//Trans1->SetTranslation(2, 2, 2);

	//m_Scene->Save(true);

	//m_Scene->AddView<PlanarSceneView>();
	//m_View = std::make_shared<PlanarView>();
	//m_View->SetCamera(&m_Scene->GetMainCamera());
	//m_View->SetViewport(RHIViewport(m_Window->GetWidth(), m_Window->GetHeight()));
	//m_View->SetScissorRect(RHIScissorRect(m_Window->GetWidth(), m_Window->GetHeight()));

	//m_RenderGraph = RenderGraph::Create(GetRenderSettings(), *m_View);

	InitializeImGui();
}

void RenderTest::InitializeImGui()
{
	if (!m_Window || !m_ViewportClient || !m_ViewportClient->GetSwapchain())
	{
		LOG_ERROR(LogDefault, "RenderTest: no window or swapchain, the editor UI stays disabled.");
		return;
	}

	auto Swapchain = static_cast<const VulkanSwapchain*>(m_ViewportClient->GetSwapchain());

	/// ImGuiEditor owns the context: its constructor creates it and loads the fonts and the theme
	/// out of Assets/Configs/EditorSettings.json. Creating a context here as well would just orphan
	/// it and leave the renderer sampling an empty atlas.
	m_Editor = std::make_unique<ImGuiEditor>(m_Window->GetWidth(), m_Window->GetHeight());

	IMGUI_CHECKVERSION();

	ImGuiIO& IO = ImGui::GetIO();
	IO.DisplaySize = ImVec2(static_cast<float>(m_Window->GetWidth()), static_cast<float>(m_Window->GetHeight()));
	IO.DeltaTime = 1.0f / 60.0f;

	/// Right away rather than waiting for UpdateDisplay(): this is what rasterizes the glyphs at the
	/// density they will be drawn at, and the renderer below picks that atlas up for good.
	m_DisplayScale = QueryDisplayScale(m_Window->GetDpiScale());
	m_Editor->SetDisplayScale(m_DisplayScale);
	IO.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	/// Nothing ever populated m_MessageHandlers before, so without this every input event was
	/// dispatched into the void and the UI could never be interacted with.
	AddMessageHandler(this);

	if (!m_ImGuiRenderer.Init(GetRenderDevice(), Swapchain->GetColorFormat()))
	{
		LOG_ERROR(LogDefault, "RenderTest: failed to initialize the Vulkan ImGui renderer, the UI stays disabled.");
	}
}

void RenderTest::UpdateDisplay()
{
	if (!m_Window || !m_ViewportClient)
	{
		return;
	}

	auto Swapchain = m_ViewportClient->GetSwapchain();
	if (!Swapchain)
	{
		return;
	}

	const float DpiScale = QueryDisplayScale(m_Window->GetDpiScale());
	const uint32_t Width = m_Window->GetWidth();
	const uint32_t Height = m_Window->GetHeight();

	if (ImGui::GetCurrentContext() && Width != 0u && Height != 0u)
	{
		ImGuiIO& IO = ImGui::GetIO();
		/// Win32 hands the client area over in device pixels once the process is per monitor DPI
		/// aware, and the swap chain has to match that 1:1. The density only shows up in the font
		/// atlas and the style (see SetDisplayScale below), never as a factor on these sizes.
		IO.DisplaySize = ImVec2(static_cast<float>(Width), static_cast<float>(Height));
		IO.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	}

	/// The swap chain mirrors the client area in device pixels; a per monitor DPI aware window
	/// reports exactly those through WM_SIZE. Rebuilding it invalidates the images the UI renderer
	/// cached views for, so those have to go first - Vulkan insists a view dies before the image it
	/// came from.
	if (Width != 0u && Height != 0u)
	{
		if (Swapchain->GetWidth() != Width || Swapchain->GetHeight() != Height)
		{
			LOG_INFO(LogDefault, "DPI: rebuilding swapchain {}x{} -> {}x{} (window {}x{} logical, scale {:.2f})",
				Swapchain->GetWidth(), Swapchain->GetHeight(),
				Width, Height, Width, Height, DpiScale);

			m_ImGuiRenderer.InvalidateBackBufferCache();
			Swapchain->Resize(Width, Height);
		}
	}

	if (m_Editor && m_Editor->SetDisplayScale(DpiScale))
	{
		/// Only now does the atlas also exist at its new size on the GPU.
		LOG_INFO(LogDefault, "DPI: rebuilt font atlas and style for scale {:.2f}", DpiScale);
		m_ImGuiRenderer.RebuildFontAtlas(GetRenderDevice());
	}
}

void RenderTest::DrawFrame()
{
	UpdateDisplay();

	auto Swapchain = m_ViewportClient->GetSwapchain();
	if (!Swapchain)
	{
		return;
	}

	Swapchain->AdvanceFrame();

	auto CommandListContext = GetRenderDevice().GetImmediateCommandListContext(ERHIDeviceQueue::Graphics);
	auto CommandBuffer = CommandListContext->GetGraphicsCommandBuffer();

	CommandBuffer->ClearColorTexture(Swapchain->GetBackBuffer(), Math::Color(0.12f, 0.12f, 0.14f, 1.0f));

	if (m_Editor)
	{
		ImGui::NewFrame();
		m_Editor->Draw();

		/// ImGui::Render() only produces the draw data, this is what records it into the command
		/// buffer. It draws over the colour ClearColorTexture() just wrote and leaves the back
		/// buffer in the layout TransitionToPresent() expects.
		ImGui::Render();
		m_ImGuiRenderer.RenderDrawData(CommandBuffer, Swapchain->GetBackBuffer());

		/// Validation output at 1 Hz: the invariant to hold is swapchain == window * scale while
		/// the draw data keeps flowing; a zeroed or frozen draw data line means the UI is not
		/// submitting anything.
		static auto LastStatus = std::chrono::steady_clock::now();
		if (std::chrono::steady_clock::now() - LastStatus > std::chrono::seconds(1))
		{
			LastStatus += std::chrono::seconds(1);

			const ImDrawData* DrawData = ImGui::GetDrawData();
			LOG_INFO(LogDefault,
				"DPI: window {}x{} device px @ {:.2f}, swapchain {}x{}, ImGui display {}x{}, draw {} vtx / {} idx",
				m_Window->GetWidth(), m_Window->GetHeight(), m_DisplayScale,
				Swapchain->GetWidth(), Swapchain->GetHeight(),
				ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y,
				DrawData ? DrawData->TotalVtxCount : 0, DrawData ? DrawData->TotalIdxCount : 0);
		}
	}

	Swapchain->TransitionToPresent(CommandBuffer);

	CommandListContext->SubmitGraphicsCommandBuffer();

	Swapchain->Present();
}

void RenderTest::Render()
{
	DrawFrame();
}

bool RenderTest::OnKeyDown(const KeyEvent& Event, const KeyModifiers& /* Modifiers */)
{
	if (!ImGui::GetCurrentContext())
	{
		return false;
	}

	/// ImGui derives io.KeyCtrl/Shift/Alt from the individual modifier key events, so the
	/// KeyModifiers carried by the event do not need to be forwarded separately.
	const ImGuiKey Key = ToImGuiKey(Event.Key);
	if (Key != ImGuiKey_None)
	{
		ImGui::GetIO().AddKeyEvent(Key, true);
	}

	return ImGui::GetIO().WantCaptureKeyboard;
}

bool RenderTest::OnKeyUp(const KeyEvent& Event, const KeyModifiers& /* Modifiers */)
{
	if (!ImGui::GetCurrentContext())
	{
		return false;
	}

	const ImGuiKey Key = ToImGuiKey(Event.Key);
	if (Key != ImGuiKey_None)
	{
		ImGui::GetIO().AddKeyEvent(Key, false);
	}

	return ImGui::GetIO().WantCaptureKeyboard;
}

bool RenderTest::OnKeyChar(const char Character)
{
	if (!ImGui::GetCurrentContext())
	{
		return false;
	}

	ImGui::GetIO().AddInputCharacter(static_cast<unsigned int>(static_cast<unsigned char>(Character)));

	return ImGui::GetIO().WantCaptureKeyboard;
}

bool RenderTest::OnMouseButtonDown(const EMouseButton Button, const Math::Vector2& CursorPos)
{
	if (!ImGui::GetCurrentContext())
	{
		return false;
	}

	ImGuiIO& IO = ImGui::GetIO();
	IO.AddMousePosEvent(CursorPos.x, CursorPos.y);

	const int MouseButton = ToImGuiMouseButton(Button);
	if (MouseButton >= 0)
	{
		IO.AddMouseButtonEvent(MouseButton, true);
	}

	return IO.WantCaptureMouse;
}

bool RenderTest::OnMouseButtonUp(const EMouseButton Button)
{
	if (!ImGui::GetCurrentContext())
	{
		return false;
	}

	const int MouseButton = ToImGuiMouseButton(Button);
	if (MouseButton >= 0)
	{
		ImGui::GetIO().AddMouseButtonEvent(MouseButton, false);
	}

	return ImGui::GetIO().WantCaptureMouse;
}

bool RenderTest::OnMouseMove(const Math::Vector2& CursorPos)
{
	if (!ImGui::GetCurrentContext())
	{
		return false;
	}

	ImGui::GetIO().AddMousePosEvent(CursorPos.x, CursorPos.y);

	return ImGui::GetIO().WantCaptureMouse;
}

bool RenderTest::OnMouseWheel(const float WheelDelta, const Math::Vector2& CursorPos)
{
	if (!ImGui::GetCurrentContext())
	{
		return false;
	}

	ImGuiIO& IO = ImGui::GetIO();
	IO.AddMousePosEvent(CursorPos.x, CursorPos.y);

	/// WindowsApplication normalizes the raw delta so one notch of the wheel is 1.0f, which is what
	/// ImGui expects.
	IO.AddMouseWheelEvent(0.0f, WheelDelta);

	return IO.WantCaptureMouse;
}

void RenderTest::Finalize()
{
	RemoveMessageHandler(this);

	/// Has to happen while the device is still alive, which it is: Finalize() runs before
	/// BaseApplication::Run() tears the RHI down.
	m_ImGuiRenderer.Shutdown();

	m_Editor.reset();

	if (ImGui::GetCurrentContext())
	{
		ImGui::DestroyContext();
	}
}

REGISTER_APPLICATION(RenderTest);
