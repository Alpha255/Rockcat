#include "Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"
#include "Engine/Scene/Components/StaticMesh.h"

struct PreDepthPassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, DepthOnly>
{
	PreDepthPassMeshDrawCommandBuilder(const GraphicsSettings& InGfxSettings)
		: GeometryPassMeshDrawCommandBuilder(InGfxSettings)
	{
		PassShader.VertexShader.SetDefine("_HAS_NORMAL_", false);

		GfxPipelineCreateInfo.DepthStencilState.SetEnableDepth(true)
			.SetEnableDepthWrite(true)
			.SetDepthCompareFunc(GfxSettings.InverseDepth ? ERHICompareFunc::LessOrEqual : ERHICompareFunc::GreaterOrEqual);

		GfxPipelineCreateInfo.SetShader(&PassShader.VertexShader)
			.SetShader(&PassShader.FragmentShader);
	}

	MeshDrawCommand Build(const StaticMesh& Mesh, const Scene& InScene) override final
	{
		auto Command = MeshDrawCommand(Mesh);

		uint16_t Location = 0u;
		EVertexAttributes DepthOnlyVertexAttributes = EVertexAttributes::Position;
		bool SupportTexel = false;

		if (auto Buffer = Mesh.GetVertexBuffer(DepthOnlyVertexAttributes))
		{
			Command.AddVertexStream(Location++, 0u, Buffer);
		}
		if (auto Buffer = Mesh.GetVertexBuffer(EVertexAttributes::UV0))
		{
			Command.AddVertexStream(Location++, 0u, Buffer);
			SupportTexel = true;
			DepthOnlyVertexAttributes = DepthOnlyVertexAttributes | EVertexAttributes::UV0;
			PassShader.VertexShader.SetDefine("_HAS_UV0_", true);
		}

		return Command;
	}

	RHIGraphicsPipelineCreateInfo GfxPipelineCreateInfo;
};

PreDepthPass::PreDepthPass(DAGNodeID ID, RenderGraph& Graph)
	: GeometryPass(ID, "PreDepthPass", Graph, EGeometryPassFilter::PreDepth, new PreDepthPassMeshDrawCommandBuilder(Graph.GetGraphicsSettings()))
{
}

void PreDepthPass::Compile()
{
	//auto& GfxSettings = RHIInterface::GetGraphicsSettings();

	//AddOutput("SceneDepth").CreateAsTexture()
	//	.SetWidth(GfxSettings.Resolution.Width)
	//	.SetHeight(GfxSettings.Resolution.Height)
	//	.SetDimension(ERHITextureDimension::T_2D)
	//	.SetFormat(ERHIFormat::D32_Float_S8_UInt)
	//	.SetUsages(ERHIBufferUsageFlags::DepthStencil);

	//auto DepthStencilState = RHIDepthStencilStateCreateInfo()
	//	.SetEnableDepth(true)
	//	.SetEnableDepthWrite(true)
	//	.SetDepthCompareFunc(GfxSettings.InverseDepth ? ERHICompareFunc::LessOrEqual : ERHICompareFunc::GreaterOrEqual);
}
