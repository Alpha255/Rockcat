#include "Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"
#include "Engine/Scene/Components/StaticMesh.h"
#include "Engine/RHI/RHIInterface.h"
#include "Engine/RHI/RHIDevice.h"

struct PreDepthPassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, DepthOnly>
{
	PreDepthPassMeshDrawCommandBuilder(RHIInterface& InBackend)
		: GeometryPassMeshDrawCommandBuilder(InBackend)
	{
		PassShader.VertexShader.SetDefine("_HAS_NORMAL_", false);

		GfxPipelineCreateInfo.DepthStencilState.SetEnableDepth(true)
			.SetEnableDepthWrite(true)
			.SetDepthCompareFunc(InBackend.GetGraphicsSettings().InverseDepth ? ERHICompareFunc::LessOrEqual : ERHICompareFunc::GreaterOrEqual);

		GfxPipelineCreateInfo.SetShader(&PassShader.VertexShader)
			.SetShader(&PassShader.FragmentShader);
		
		GfxPipelineCreateInfo.RenderPassCreateInfo.SetDepthStencilAttachment(ERHIFormat::D32_Float_S8_UInt);
	}

	MeshDrawCommand Build(const StaticMesh& Mesh, const Scene& InScene) override final
	{
		auto Command = MeshDrawCommand(Mesh);

		uint16_t Location = 0u;
		EVertexAttributes DepthOnlyVertexAttributes = EVertexAttributes::Position;
		bool SupportTexel = false;

		if (auto Buffer = Mesh.GetVertexBuffer(DepthOnlyVertexAttributes))
		{
			Command.VertexStream.Add(Location++, 0u, Buffer);
		}
		if (auto Buffer = Mesh.GetVertexBuffer(EVertexAttributes::UV0))
		{
			Command.VertexStream.Add(Location++, 0u, Buffer);
			SupportTexel = true;
			DepthOnlyVertexAttributes = DepthOnlyVertexAttributes | EVertexAttributes::UV0;
			PassShader.VertexShader.SetDefine("_HAS_UV0_", true);
		}

		if (LastVertexAttributes != DepthOnlyVertexAttributes || LastPrimitiveTopology != Mesh.GetPrimitiveTopology())
		{
			GfxPipelineCreateInfo.InputLayout = MeshData::GetInputLayoutCreateInfo(DepthOnlyVertexAttributes);
			LastVertexAttributes = DepthOnlyVertexAttributes;

			LastPrimitiveTopology = Mesh.GetPrimitiveTopology();
			GfxPipelineCreateInfo.SetPrimitiveTopology(Mesh.GetPrimitiveTopology());

			Command.GraphicsPipeline = Backend.GetDevice().GetOrCreateGraphicsPipeline(GfxPipelineCreateInfo);
			LastGraphicsPipeline = Command.GraphicsPipeline;
		}
		else
		{
			assert(LastGraphicsPipeline);
			Command.GraphicsPipeline = LastGraphicsPipeline;
		}

		return Command;
	}

	RHIGraphicsPipelineCreateInfo GfxPipelineCreateInfo;
	EVertexAttributes LastVertexAttributes = EVertexAttributes::Num;
	ERHIPrimitiveTopology LastPrimitiveTopology = ERHIPrimitiveTopology::Num;
	RHIGraphicsPipeline* LastGraphicsPipeline = nullptr;
};

PreDepthPass::PreDepthPass(DAGNodeID ID, RenderGraph& Graph)
	: GeometryPass(ID, "PreDepthPass", Graph, EGeometryPassFilter::PreDepth, new PreDepthPassMeshDrawCommandBuilder(Graph.GetBackend()))
{
}

void PreDepthPass::Compile()
{
	auto& GfxSettings = GetGraphicsSettings();

	AddOutput(RDGResource::EType::Texture, SceneTextures::SceneDepth).GetTextureCreateInfo()
		.SetWidth(GfxSettings.Resolution.Width)
		.SetHeight(GfxSettings.Resolution.Height)
		.SetDimension(ERHITextureDimension::T_2D)
		.SetFormat(ERHIFormat::D32_Float_S8_UInt)
		.SetUsages(ERHIBufferUsageFlags::DepthStencil);
}
