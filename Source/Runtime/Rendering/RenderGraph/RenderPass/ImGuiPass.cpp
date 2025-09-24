#include "Rendering/RenderGraph/RenderPass/ImGuiPass.h"
#include <imgui/imgui.h>

ImGUIPass::~ImGUIPass()
{
	ImGui::DestroyContext();
}

#if 0
#include "Colorful/IRenderer/RenderGraph/RenderPass/ImGUIPass.h"
#include "Colorful/IRenderer/IRenderer.h"
#include "Asset/Material.h"
#include "Asset/Model.h"

NAMESPACE_START(RHI)

ImGUIPass::ImGUIPass(const Scene* TargetScene, const Camera* ViewCamera)
	: IFrameGraphPass(TargetScene, ViewCamera)
{
	m_Device = IRenderer::Get().Device();
	assert(m_Device);

	IMGUI_CHECKVERSION();
	m_Context = ImGui::CreateContext();

	m_Material = Material::Load("Imgui.json");

	m_GraphicsPipelineDesc.Shaders = m_Material->PipelineShaders();
	m_GraphicsPipelineDesc.BlendState.RenderTargetBlends[0]
		.SetIndex(0u)
		.SetEnable(true)
		.SetColorMask(EColorWriteMask::All)
		.SetSrcColorBlendFactor(EBlendFactor::SrcAlpha)
		.SetDstColorBlendFactor(EBlendFactor::InverseSrcAlpha)
		.SetColorBlendOp(EBlendOp::Add)
		.SetSrcAlphaBlendFactor(EBlendFactor::InverseSrcAlpha)
		.SetDstAlphaBlendFactor(EBlendFactor::Zero)
		.SetAlphaBlendOp(EBlendOp::Add);

	m_GraphicsPipelineDesc.DepthStencilState.SetEnableDepth(false);
	m_GraphicsPipelineDesc.RasterizationState.SetCullMode(ECullMode::None);

	InputLayoutDesc LayoutDesc;
	LayoutDesc.Bindings.emplace_back(InputLayoutDesc::VertexInputBinding
		{
			0u,
			0u,
			EVertexInputRate::Vertex
		});
	LayoutDesc.Bindings[0]
		.AddAttribute(InputLayoutDesc::VertexAttribute
			{
				0u,
				sizeof(ImDrawVert::pos),
				EFormat::RG32_Float,
				"POSITION"
			})
		.AddAttribute(InputLayoutDesc::VertexAttribute
			{
				1u,
				sizeof(ImDrawVert::uv),
				EFormat::RG32_Float,
				"TECOORD0"
			})
		.AddAttribute(InputLayoutDesc::VertexAttribute
			{
				2u,
				sizeof(ImDrawVert::col),
				EFormat::RGBA8_UNorm,
				"COLOR0"
			});

	m_InputLayout = m_Device->GetOrCreateInputLayout(
		LayoutDesc,
		*(m_GraphicsPipelineDesc.Shaders.Get(EShaderStage::Vertex)->Desc())
	);
	m_GraphicsPipelineDesc.InputLayout = m_InputLayout.get();

	uint8_t* Pixels = nullptr;
	int32_t Width = 0;
	int32_t Height = 0;
	m_Context->IO.Fonts->GetTexDataAsRGBA32(&Pixels, &Width, &Height);
	ImageDesc ImageCreationDesc;
	ImageCreationDesc
		.SetWidth(static_cast<uint32_t>(Width))
		.SetHeight(static_cast<uint32_t>(Height))
		.SetImageType(EImageType::T_2D)
		.SetFormat(EFormat::RGBA8_UNorm)
		.SetSampleCount(ESampleCount::Sample_1_Bit)
		.SetUsages(EBufferUsageFlags::ShaderResource)
		.SetRequiredState(EResourceState::ShaderResource)
		.SetName("ImGUIFontImage")
		.SetData(Pixels)
		.SetDataSize(Width * Height * 4u);
	ImageCreationDesc.Subresources.resize(1u);
	ImageCreationDesc.Subresources[0] = ImageSubresourceDesc
	{
		static_cast<uint32_t>(Width),
		static_cast<uint32_t>(Height),
		1u,
		0u,
		0u,
		0u,
		Width * 4u,
		Width * Height * 4u
	};

	m_FontImage = m_Device->CreateImage(ImageCreationDesc);

	SamplerDesc SamplerCreationDesc;
	SamplerCreationDesc
		.SetMinMagFilter(EFilter::Linear)
		.SetMipmapMode(EFilter::Linear);
	m_LinearSampler = m_Device->GetOrCreateSampler(SamplerCreationDesc);

	m_UniformBuffer = m_Device->CreateUniformBuffer(sizeof(Vector2));

	CustomStyles();
}

void ImGUIPass::ApplyRenderSettings(const RenderSettings* Settings)
{
	m_Context->IO.DisplaySize = ImVec2(static_cast<float>(Settings->Resolution.Width), static_cast<float>(Settings->Resolution.Height));

	GraphicsPipelineDesc Desc = m_GraphicsPipelineDesc;
	Desc.FrameBuffer = m_SwapchainFrameBuffer;
	m_GraphicsPipeline = m_Device->GetOrCreateGraphicsPipeline(Desc);
}

void ImGUIPass::Render(ICommandBuffer* Command)
{
	assert(Command);

	SCOPED_STATS(ImGUIPass);

	SCOPED_RENDER_EVENT(Command, ImGUIPass, Color::Random());

	ImGui::NewFrame();

	if (m_BuildWidgetsFunc)
	{
		m_BuildWidgetsFunc();
	}

	ImGui::Render();

	FlushDrawData(Command);

	auto State = m_GraphicsPipeline->State();

	State->SetVertexBuffer(m_VertexBuffer.get());

	State->SetIndexBuffer(m_IndexBuffer.get(), sizeof(ImDrawIdx) == sizeof(uint16_t) ? EIndexFormat::UInt16 : EIndexFormat::UInt32);

	State->SetImage<EShaderStage::Fragment>(0u, m_FontImage);

	State->SetSampler<EShaderStage::Fragment>(1u, m_LinearSampler);

	State->SetViewport(Viewport(m_Context->IO.DisplaySize.x, m_Context->IO.DisplaySize.y));

	Command->SetGraphicsPipeline(m_GraphicsPipeline.get());

	Vector2 InvserseDisplaySize(1.0f / m_Context->IO.DisplaySize.x, 1.0f / m_Context->IO.DisplaySize.y);
#if 0
	Command->CopyBuffer(m_UniformBuffer.get(), &TranslateScale, sizeof(Vector4));
#else
	Command->PushConstants(EShaderStage::Vertex, m_UniformBuffer.get(), &InvserseDisplaySize, sizeof(Vector2));
#endif

	for (int32_t CmdIndex = 0, OffsetV = 0, OffsetI = 0; CmdIndex < ImGui::GetDrawData()->CmdListsCount; ++CmdIndex)
	{
		const ImDrawList* DrawList = ImGui::GetDrawData()->CmdLists[CmdIndex];
		for (int32_t CmdBufferIndex = 0; CmdBufferIndex < DrawList->CmdBuffer.Size; ++CmdBufferIndex)
		{
			const ImDrawCmd* DrawCmd = &DrawList->CmdBuffer[CmdBufferIndex];
			/// ClipRect.x minX, ClipRect.y minY, ClipRect.z maxX, ClipRect.w maxY
			Command->SetScissorRect(ScissorRect(
				DrawCmd->ClipRect.x, 
				DrawCmd->ClipRect.y, 
				DrawCmd->ClipRect.z - DrawCmd->ClipRect.x, 
				DrawCmd->ClipRect.w - DrawCmd->ClipRect.y));

			Command->DrawIndexed(DrawCmd->ElemCount, static_cast<uint32_t>(OffsetI), OffsetV);
			OffsetI += DrawCmd->ElemCount;
		}
		OffsetV += DrawList->VtxBuffer.Size;
	}
}

void ImGUIPass::OnMouseEvent(const MouseEvent& Mouse)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos.x = Mouse.Position.x;
	io.MousePos.y = Mouse.Position.y;
	io.MouseWheel += Mouse.WheelDelta;

	switch (Mouse.Button)
	{
	case EMouseButton::LButton:
		io.MouseDown[0] = Mouse.State == EKeyState::Down;
		break;
	case EMouseButton::RButton:
		io.MouseDown[1] = Mouse.State == EKeyState::Down;
		break;
	case EMouseButton::MButton:
		io.MouseDown[2] = Mouse.State == EKeyState::Down;
		break;
	}
}

void ImGUIPass::OnKeyboardEvent(const KeyboardEvent& Keyboard)
{
	ImGuiIO& IO = ImGui::GetIO();

	if (Keyboard.InputChar)
	{
		IO.AddInputCharacter(Keyboard.InputChar);
	}

	if (Keyboard.Key != EKeyboardKey::None)
	{
		IO.KeysDown[static_cast<int32_t>(Keyboard.Key)] = Keyboard.State == EKeyState::Down;
	}
}

bool ImGUIPass::WantCaptureInput() const
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

void ImGUIPass::OnWindowResized(uint32_t Width, uint32_t Height)
{
	m_Context->IO.DisplaySize = ImVec2(static_cast<float>(Width), static_cast<float>(Height));
}

void ImGUIPass::Tick(float ElapsedSeconds)
{
	ImGui::GetIO().DeltaTime = ElapsedSeconds;
}

void ImGUIPass::FlushDrawData(ICommandBuffer* Command)
{
	const ImDrawData* DrawData = ImGui::GetDrawData();
	if (DrawData && DrawData->CmdListsCount > 0)
	{
		if (m_LastVertexCount < DrawData->TotalVtxCount)
		{
			m_LastVertexCount = DrawData->TotalVtxCount + 5000;
			m_VertexBuffer = m_Device->CreateVertexBuffer(sizeof(ImDrawVert) * m_LastVertexCount, EDeviceAccessFlags::GpuReadCpuWrite, nullptr);
		}

		if (m_LastIndexCount < DrawData->TotalIdxCount)
		{
			m_LastIndexCount = DrawData->TotalIdxCount + 5000;
			m_IndexBuffer = m_Device->CreateIndexBuffer(sizeof(ImDrawIdx) * m_LastIndexCount, EDeviceAccessFlags::GpuReadCpuWrite, nullptr);
		}

		size_t VertexOffset = 0u;
		size_t IndexOffset = 0u;
		for (int32_t Index = 0; Index < DrawData->CmdListsCount; ++Index)
		{
			const ImDrawList* DrawList = DrawData->CmdLists[Index];
			size_t VertexSize = DrawList->VtxBuffer.Size * sizeof(ImDrawVert);
			size_t IndexSize = DrawList->IdxBuffer.Size * sizeof(ImDrawIdx);

			Command->CopyBuffer(m_VertexBuffer.get(), DrawList->VtxBuffer.Data, VertexSize, 0u, VertexOffset);
			Command->CopyBuffer(m_IndexBuffer.get(), DrawList->IdxBuffer.Data, IndexSize, 0u, IndexOffset);

			VertexOffset += VertexSize;
			IndexOffset += IndexSize;
		}
	}
}

void ImGUIPass::CustomStyles()
{
	ImGuiStyle& Style = ImGui::GetStyle();
	Style.WindowRounding = 5.0f;
	Style.ChildRounding = 3.0f;
	Style.PopupRounding = 3.0f;
	Style.FrameRounding = 3.0f;
	Style.ScrollbarRounding = 3.0f;

#define MAP_KEY(ImGui_Key, Key) IO.KeyMap[ImGui_Key] = static_cast<int32_t>(Key)
	ImGuiIO& IO = ImGui::GetIO();
	MAP_KEY(ImGuiKey_Tab, EKeyboardKey::Tab);
	MAP_KEY(ImGuiKey_Enter, EKeyboardKey::Enter);
	MAP_KEY(ImGuiKey_LeftShift, EKeyboardKey::Shift);
	MAP_KEY(ImGuiKey_LeftCtrl, EKeyboardKey::Ctrl);
	MAP_KEY(ImGuiKey_Escape, EKeyboardKey::Escape);
	MAP_KEY(ImGuiKey_LeftAlt , EKeyboardKey::Alt);
	MAP_KEY(ImGuiKey_PageUp, EKeyboardKey::PageUp);
	MAP_KEY(ImGuiKey_PageDown, EKeyboardKey::PageDown);
	MAP_KEY(ImGuiKey_Home, EKeyboardKey::Home);
	MAP_KEY(ImGuiKey_LeftArrow, EKeyboardKey::Left);
	MAP_KEY(ImGuiKey_RightArrow, EKeyboardKey::Right);
	MAP_KEY(ImGuiKey_UpArrow, EKeyboardKey::Up);
	MAP_KEY(ImGuiKey_DownArrow, EKeyboardKey::Down);
#undef MAP_KEY

	ImGui::StyleColorsClassic();
}

NAMESPACE_END(RHI)

#endif
