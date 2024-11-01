#include "Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/RHI/RHIInterface.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"

void PreDepthPass::Compile()
{
	auto& GraphicsSettings = RHIInterface::GetGraphicsSettings();

	AddOutput("SceneDepth").CreateAsTexture()
		.SetWidth(GraphicsSettings.Resolution.Width)
		.SetHeight(GraphicsSettings.Resolution.Height)
		.SetDimension(ERHITextureDimension::T_2D)
		.SetFormat(ERHIFormat::D32_Float_S8_UInt)
		.SetUsages(ERHIBufferUsageFlags::DepthStencil);

	auto DepthStencilState = RHIDepthStencilStateCreateInfo()
		.SetEnableDepth(true)
		.SetEnableDepthWrite(true)
		.SetDepthCompareFunc(GraphicsSettings.InverseDepth ? ERHICompareFunc::LessOrEqual : ERHICompareFunc::GreaterOrEqual);

	auto GraphicsPipelineDesc = RHIGraphicsPipelineCreateInfo()
		.SetDepthStencilState(DepthStencilState)
		.SetShader(GenericVS())
		.SetShader(DepthOnly());

	m_GraphicsPipeline = GetResourceManager().GetOrCreateGraphicsPipeline(GraphicsPipelineDesc);
}
