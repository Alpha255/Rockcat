#include "Colorful/IRenderer/RenderGraph/RenderPass/BlitPass.h"
#include "Colorful/IRenderer/RenderGraph/RenderPass/ForwardShadingPass.h"
#include "Colorful/IRenderer/IRenderer.h"
#include "Runtime/Asset/AssetDatabase.h"
#include "Runtime/Scene.h"

NAMESPACE_START(RHI)

ImageSlice BlitPass::AllSlice;

BlitPass::BlitPass(const Scene* TargetScene, const Camera* ViewCamera)
	: IFrameGraphPass(TargetScene, ViewCamera)
{
	m_FullscreenTriangle = MeshInstance::FullscreenTriangle();
	m_FullscreenQuad = MeshInstance::FullscreenQuad();

	SetMode(m_Mode);

	if (auto Device = IRenderer::Get().Device())
	{
		SamplerDesc Desc;
		Desc.MinMagFilter = Desc.MipmapMode = EFilter::Linear;
		m_LinearSampler = Device->CreateSampler(Desc);

		Desc.MinMagFilter = Desc.MipmapMode = EFilter::Nearest;
		m_PointSampler = Device->CreateSampler(Desc);
	}
}

void BlitPass::SetMode(EMode Mode)
{
	assert(Mode != EMode::CopyImage);

	m_Mode = Mode;

	if (m_Mode == EMode::FullscreenTriangle)
	{
		m_Drawable.Mesh = m_FullscreenTriangle.get();
	}
	else if (m_Mode == EMode::FullscreenQuad)
	{
		m_Drawable.Mesh = m_FullscreenQuad.get();
	}

	m_FullscreenTriangle->GetMaterial()->SwitchPermutation(RHI::EShaderStage::Vertex, m_Mode);
}

void BlitPass::ApplyRenderSettings(const RenderSettings* Settings)
{
	if (m_Mode != EMode::CopyImage)
	{
		GraphicsPipelineDesc Desc;
		if (m_Mode == EMode::FullscreenQuad || m_Mode == EMode::FullscreenTriangle)
		{
			Desc.PrimitiveTopology = m_FullscreenTriangle->GetMesh()->PrimitiveTopology();
			Desc.InputLayout = m_FullscreenTriangle->GetMesh()->InputLayout();
		}
		else if (m_Mode == EMode::GpuGenFullscreenQuad || m_Mode == EMode::GpuGenFullscreenTriangle)
		{
			Desc.PrimitiveTopology = EPrimitiveTopology::TriangleStrip;
		}

		Desc.FrameBuffer = m_SwapchainFrameBuffer;
		Desc.Shaders = m_FullscreenTriangle->GetMaterial()->PipelineShaders();
		Desc.BlendState.RenderTargetBlends[0].Index = 0u;

		auto& Pipeline = m_GraphicsPipelines[Desc.Hash()];
		if (!Pipeline)
		{
			Pipeline = IRenderer::Get().Device()->GetOrCreateGraphicsPipeline(Desc);
		}

		m_GraphicsPipeline = Pipeline;
	}

	AllSlice.Width = Settings->Resolution.Width;
	AllSlice.Height = Settings->Resolution.Height;

	IFrameGraphPass::ApplyRenderSettings(Settings);

	m_BlitSrc = static_cast<ForwardShadingPass*>(m_BindPass)->GetColorAttachment();
}

void BlitPass::SetupMaterial(const MeshInstance*)
{
	if (m_Mode != EMode::CopyImage)
	{
		auto State = m_GraphicsPipeline->State();

		State->SetImage<EShaderStage::Fragment>(0u, m_BlitSrc);

		State->SetSampler<EShaderStage::Fragment>(1u, m_Sampler == ESampler::Linear ? m_LinearSampler : m_PointSampler);
	}
}

void BlitPass::Render(ICommandBuffer* Command)
{
	assert(Command && m_BlitSrc);

	SCOPED_STATS(BlitPass);

	SCOPED_RENDER_EVENT(Command, BlitPass, Color::Random());

	if (m_Mode == EMode::CopyImage)
	{
		Command->CopyImage(
			m_BlitSrc.get(),
			AllSlice, 
			m_SwapchainFrameBuffer->Description().ColorAttachments[0].get(), 
			AllSlice);
	}
	else if (m_Mode == EMode::FullscreenQuad || m_Mode == EMode::FullscreenTriangle)
	{
		Draw(Command, m_Drawable);
	}
	else
	{
		SetupMaterial(nullptr);

		auto State = m_GraphicsPipeline->State();

		State->SetViewport(m_Viewport);

		State->SetScissorRect(m_Scissor);

		Command->SetGraphicsPipeline(m_GraphicsPipeline.get());

		Command->Draw(m_Mode == EMode::GpuGenFullscreenQuad ? 4u : 3u);
	}
}

NAMESPACE_END(RHI)
