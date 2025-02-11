#include "Engine/Rendering/RenderGraph/RenderPass/PreDepthPass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Asset/GlobalShaders.h"
#include "Engine/Scene/Components/StaticMesh.h"
#include "Engine/RHI/RHIBackend.h"
#include "Engine/RHI/RHIDevice.h"

struct PreDepthPassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, DepthOnlyFS>
{
	PreDepthPassMeshDrawCommandBuilder(RHIBackend& InBackend)
		: GeometryPassMeshDrawCommandBuilder(InBackend)
	{
		PassShader.VertexShader.SetDefine("_HAS_NORMAL_", false);

		GfxPipelineCreateInfo.DepthStencilState.SetEnableDepth(true)
			.SetEnableDepthWrite(true)
			.SetDepthCompareFunc(false ? ERHICompareFunc::LessOrEqual : ERHICompareFunc::GreaterOrEqual);

		GfxPipelineCreateInfo.SetShader(&PassShader.VertexShader)
			.SetShader(&PassShader.FragmentShader);
		
		GfxPipelineCreateInfo.RenderPassCreateInfo.SetDepthStencilAttachment(ERHIFormat::D32_Float_S8_UInt);

		//auto& GfxSettings = Backend.GetGraphicsSettings();
		//RHIViewport Viewport(GfxSettings.Resolution.Width, GfxSettings.Resolution.Height);
		//RHIScissorRect ScissorRect(GfxSettings.Resolution.Width, GfxSettings.Resolution.Height);

		//GfxPipelineCreateInfo.SetViewport(Viewport)
		//	.SetScissorRect(ScissorRect);
	}

	MeshDrawCommand Build(const StaticMesh& Mesh, const Scene&) override final
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
	: GeometryPass(ID, "PreDepthPass", Graph)
{
}

void PreDepthPass::Compile()
{
	AddOutput(RDGResource::EType::Texture, SceneTextures::SceneDepth).GetTextureCreateInfo()
		.SetWidth(0)
		.SetHeight(0)
		.SetDimension(ERHITextureDimension::T_2D)
		.SetFormat(ERHIFormat::D32_Float_S8_UInt)
		.SetUsages(ERHIBufferUsageFlags::DepthStencil);
}
