#include "Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Asset/GlobalShaders.h"
#include "Engine/Scene/Components/StaticMesh.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/RHI/RHIDevice.h"

struct PreDepthMeshDrawCommandBuilder : public MeshDrawCommandBuilder
{
	std::shared_ptr<MeshDrawCommand> Build(const StaticMesh& Mesh, const RenderSettings& GraphicsSettings) override final
	{
		auto Command = std::make_shared<MeshDrawCommand>(Mesh);
		auto& GfxPipelineCreateInfo = Command->GraphicsPipelineCreateInfo;
		auto VertexShader = std::make_shared<GenericVS>();
		auto FragmentShader = std::make_shared<DepthOnlyFS>();
		
		GfxPipelineCreateInfo.SetPrimitiveTopology(Mesh.GetPrimitiveTopology());

		GfxPipelineCreateInfo.DepthStencilState.SetEnableDepth(true)
			.SetEnableDepthWrite(true)
			.SetDepthCompareFunc(GraphicsSettings.InverseDepth ? ERHICompareFunc::LessOrEqual : ERHICompareFunc::GreaterOrEqual);

		GfxPipelineCreateInfo.RenderPassCreateInfo.SetDepthStencilAttachment(ERHIFormat::D32_Float_S8_UInt);

		uint16_t Location = 0u;
		EVertexAttributes VertexAttributes = EVertexAttributes::Position;

		if (auto Buffer = Mesh.GetVertexBuffer(VertexAttributes))
		{
			Command->VertexStream.Add(Location++, 0u, Buffer);
		}
		if (auto Buffer = Mesh.GetVertexBuffer(EVertexAttributes::UV0))
		{
			Command->VertexStream.Add(Location++, 0u, Buffer);
			VertexAttributes = VertexAttributes | EVertexAttributes::UV0;
			VertexShader->SetDefine("_HAS_UV0_", true);
		}

		VertexShader->SetDefine("_HAS_NORMAL_", false);

		GfxPipelineCreateInfo.SetShader(VertexShader)
			.SetShader(FragmentShader);

		return Command;
	}
};

PreDepthPass::PreDepthPass(DAGNodeID ID, RenderGraph& Graph)
	: GeometryPass(ID, "PreDepthPass", Graph)
{
	RenderScene::RegisterMeshDrawCommandBuilder(EGeometryPass::PreDepth, new PreDepthMeshDrawCommandBuilder());
}

void PreDepthPass::Compile()
{
	auto Resulotion = GetGraph().GetDisplaySize();

	AddOutput(RDGResource::EType::Texture, SceneTextures::SceneDepth).GetTextureCreateInfo()
		.SetWidth(Resulotion.x)
		.SetHeight(Resulotion.y)
		.SetDimension(ERHITextureDimension::T_2D)
		.SetFormat(ERHIFormat::D32_Float_S8_UInt)
		.SetUsages(ERHIBufferUsageFlags::DepthStencil);
}
