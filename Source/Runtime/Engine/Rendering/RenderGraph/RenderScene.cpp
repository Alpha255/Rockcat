#include "Engine/Rendering/RenderGraph/RenderScene.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"
#include "Engine/Services/RenderService.h"

MeshDrawCommand::MeshDrawCommand(const StaticMesh& Mesh)
	: IndexBuffer(Mesh.GetIndexBuffer())
	, Material(Mesh.GetMaterialID())
	, FirstIndex(0u)
	, NumPrimitives(Mesh.GetNumPrimitive())
	, NumIndex(Mesh.GetNumIndex())
	, PrimitiveTopology(Mesh.GetPrimitiveTopology())
	, IndexFormat(Mesh.GetIndexFormat())
{
	VertexArgs.BaseIndex = 0u;
	VertexArgs.NumVertex = Mesh.GetNumVertex();
}

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

class OpaquePassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, DefaultLit>
{
public:
	using GeometryPassMeshDrawCommandBuilder::GeometryPassMeshDrawCommandBuilder;

	MeshDrawCommand Build(const StaticMesh& Mesh, const Scene& InScene) override final
	{
		auto Command = MeshDrawCommand(Mesh);
		return Command;
	}
};

class TransluentPassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, DefaultLit>
{
public:
	using GeometryPassMeshDrawCommandBuilder::GeometryPassMeshDrawCommandBuilder;

	MeshDrawCommand Build(const StaticMesh& Mesh, const Scene& InScene) override final
	{
		auto Command = MeshDrawCommand(Mesh);
		return Command;
	}
};

class ShadowPassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, GenericShadow>
{
public:
	using GeometryPassMeshDrawCommandBuilder::GeometryPassMeshDrawCommandBuilder;

	MeshDrawCommand Build(const StaticMesh& Mesh, const Scene& InScene) override final
	{
		auto Command = MeshDrawCommand(Mesh);
		return Command;
	}
};

RenderScene::RenderScene(const GraphicsSettings& GfxSettings)
	: m_GfxSettings(GfxSettings)
{
	m_MeshDrawCommandBuilders[(uint32_t)EGeometryPassFilter::PreDepth] = std::make_unique<PreDepthPassMeshDrawCommandBuilder>(GfxSettings);
	m_MeshDrawCommandBuilders[(uint32_t)EGeometryPassFilter::Opaque] = std::make_unique<OpaquePassMeshDrawCommandBuilder>(GfxSettings);
	m_MeshDrawCommandBuilders[(uint32_t)EGeometryPassFilter::Translucent] = std::make_unique<TransluentPassMeshDrawCommandBuilder>(GfxSettings);
	m_MeshDrawCommandBuilders[(uint32_t)EGeometryPassFilter::ShadowCast] = std::make_unique<ShadowPassMeshDrawCommandBuilder>(GfxSettings);
}

void RenderScene::BuildMeshDrawCommands(const Scene& InScene)
{
	for (auto& MeshDrawCmds : m_MeshDrawCommands)
	{
		MeshDrawCmds.clear();
		MeshDrawCmds.reserve(1024u);
	}

	auto& RHIDevice = RenderService::Get().GetBackend(m_GfxSettings.RenderHardwareInterface).GetDevice();

	if (m_GfxSettings.AsyncMeshDrawCommandsBuilding)
	{
		assert(false);
	}
	else
	{
		for (auto Node : InScene.GetVisibleNodes())
		{
			if (!Node)
			{
				continue;
			}

			if (Node->IsStaticMesh())
			{
				if (auto Mesh = InScene.GetStaticMesh(Node->GetDataIndex()))
				{
					const_cast<StaticMesh*>(Mesh)->CreateRHIBuffers(RHIDevice);

					for (uint32_t Index = 0u; Index < (uint32_t)EGeometryPassFilter::Num; ++Index)
					{
						m_MeshDrawCommands[Index].emplace_back(m_MeshDrawCommandBuilders[Index]->Build(*Mesh, InScene));
					}
				}
			}
			else if (Node->IsSkeletalMesh())
			{
				assert(false);
			}
		}
	}
}