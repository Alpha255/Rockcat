#include "Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Rendering/RenderGraph/RenderGraph.h"
#include "Asset/GlobalShaders.h"
#include "Scene/Components/StaticMesh.h"
#include "RHI/RHIBackend.h"
#include "RHI/RHIDevice.h"

struct PreDepthMeshDrawCommandBuilder : public MeshDrawCommandBuilder
{
	std::shared_ptr<MeshDrawCommand> Build(const StaticMesh& Mesh, const ISceneView& SceneView) override final
	{
		auto Command = std::make_shared<MeshDrawCommand>(Mesh);
		auto VertexShader = std::make_shared<GenericVS>();
		auto FragmentShader = std::make_shared<DepthOnlyFS>();
		auto& GfxPipelineCreateInfo = Command->GraphicsPipelineCreateInfo;
		
		GfxPipelineCreateInfo.SetPrimitiveTopology(Mesh.GetPrimitiveTopology());

		GfxPipelineCreateInfo.DepthStencilState.SetEnableDepth(true)
			.SetEnableDepthWrite(true)
			.SetDepthCompareFunc(SceneView.IsInverseDepth() ? ERHICompareFunc::LessOrEqual : ERHICompareFunc::GreaterOrEqual);

		uint16_t Location = 0u;

		if (auto Buffer = Mesh.GetVertexBuffer(EVertexAttributes::Position))
		{
			Command->VertexStream.Add(Location++, 0u, Buffer);
		}
		if (auto Buffer = Mesh.GetVertexBuffer(EVertexAttributes::UV0))
		{
			Command->VertexStream.Add(Location++, 0u, Buffer);
			VertexShader->SetDefine("_HAS_UV0_", true);
		}

		VertexShader->SetDefine("_HAS_NORMAL_", false);

		GfxPipelineCreateInfo.SetShader(VertexShader)
			.SetShader(FragmentShader);

		SetupShaderParameters(GfxPipelineCreateInfo, SceneView, Mesh.GetTransform(), Mesh.GetMaterialProperty());

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
	//auto Resulotion = GetGraph().GetDisplaySize();

	//AddOutput(RDGResource::EType::Texture, SceneTextures::SceneDepth).GetTextureCreateInfo()
	//	.SetWidth(Resulotion.x)
	//	.SetHeight(Resulotion.y)
	//	.SetDimension(ERHITextureDimension::T_2D)
	//	.SetFormat(ERHIFormat::D32_Float_S8_UInt)
	//	.SetUsages(ERHIBufferUsageFlags::DepthStencil);
}
