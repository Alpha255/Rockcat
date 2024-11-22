#include "Engine/Rendering/RenderGraph/RenderPass/OpaquePass.h"
#include "Engine/Rendering/RenderGraph/RenderGraph.h"
#include "Engine/Asset/GlobalShaders/DefaultShading.h"
#if 0
#include "Colorful/IRenderer/RenderGraph/RenderPass/ForwardShadingPass.h"
#include "Colorful/IRenderer/IRenderer.h"
#include "Scene/Scene.h"

NAMESPACE_START(RHI)

#if true
/// #Temp
struct Transform
{
	Matrix Projection;
	Matrix Model;
	Matrix View;
	Vector3 CameraPos;
};

struct Lighting
{
	Transform Trans;
	Vector4 LightPos;
};

struct ModelPos
{
	Vector3 Position;
};

struct MaterialParams
{
	float Roughness;
	float Metallic;
};

static ModelPos PushConstants_0;
static MaterialParams PushConstants_1;
#endif

ForwardShadingPass::ForwardShadingPass(const Scene* TargetScene, const Camera* ViewCamera)
	: OpaquePass(TargetScene, ViewCamera)
{
	SamplerDesc desc;
	desc.MinMagFilter = EFilter::Linear;
	desc.MipmapMode = EFilter::Linear;

	auto RHIDevice = IRenderer::Get().Device();

	m_LinearSampler = RHIDevice->GetOrCreateSampler(desc);

	m_Transform = RHIDevice->CreateUniformBuffer(sizeof(Transform));

	m_Lighting = RHIDevice->CreateUniformBuffer(sizeof(Lighting));

	///m_Camera->setLookAt(Vec3(0.0f, 0.4f, 0.4f), Vec3());
}

void ForwardShadingPass::SetupMaterial(const MeshInstance* Mesh)
{
	assert(Mesh && Mesh->GetMesh() && Mesh->GetMaterial());

	if (m_LastMaterial != Mesh->GetMaterial())
	{
		m_LastMaterial = Mesh->GetMaterial();

		m_GraphicsPipelineDesc.PrimitiveTopology = Mesh->GetMesh()->PrimitiveTopology();
		m_GraphicsPipelineDesc.InputLayout = Mesh->GetMesh()->InputLayout();
		m_GraphicsPipelineDesc.FrameBuffer = m_FrameBuffer.get();
		m_GraphicsPipelineDesc.Shaders = m_LastMaterial->PipelineShaders();
		m_GraphicsPipelineDesc.BlendState.RenderTargetBlends[0].Index = 0u;

		auto RHIdevice = IRenderer::Get().Device();

		m_GraphicsPipeline = RHIdevice->GetOrCreateGraphicsPipeline(m_GraphicsPipelineDesc);

		/// #TODO Temp
		auto State = m_GraphicsPipeline->State();

		State->SetUniformBuffer<EShaderStage::Vertex>(0u, m_Transform);

		State->SetUniformBuffer<EShaderStage::Fragment>(1u, m_Lighting);

		State->SetImage<EShaderStage::Fragment>(2u, m_LastMaterial->Image(Material::EImageType::AlbedoOrDiffuse));

		State->SetSampler<EShaderStage::Fragment>(3u, m_LinearSampler);
	}
}

void ForwardShadingPass::UpdateUniformBuffers(ICommandBuffer* Command)
{
	Transform TempTransform;
	TempTransform.Projection = m_Camera->ProjectionMatrix();
	TempTransform.View = m_Camera->ViewMatrix();
	TempTransform.Model = m_Camera->WorldMatrix();

	Command->CopyBuffer(m_Transform.get(), &TempTransform, sizeof(Transform));

	Command->PushConstants(EShaderStage::Vertex, nullptr, &PushConstants_0, sizeof(ModelPos));

	Command->PushConstants(EShaderStage::Fragment, nullptr, &PushConstants_1, sizeof(MaterialParams));
}

void ForwardShadingPass::ApplyRenderSettings(const RenderSettings* Settings)
{
	assert(Settings);

	m_EnableFrustumCulling = Settings->FrustumCulling;
	m_ShadowTechnique = Settings->ShadowTechnique;

	auto RHIDevice = IRenderer::Get().Device();

	if (!m_FrameBuffer)
	{
		auto TempFrameBufferDesc = FrameBufferDesc()
			.AddColorAttachment(RHIDevice->CreateRenderTarget(
				Settings->Resolution.Width,
				Settings->Resolution.Height,
				EFormat::RGBA16_Float,
				ESampleCount::Sample_1_Bit,
				"Forward HDRColorTarget",
				true))
			.SetDepthStencilAttachment(RHIDevice->CreateDepthStencil(
				Settings->Resolution.Width,
				Settings->Resolution.Height,
				EFormat::D24_UNorm_S8_UInt,
				"Forward DepthStencil"));

		m_FrameBuffer = RHIDevice->GetOrCreateFrameBuffer(TempFrameBufferDesc);
	}

	IFrameGraphPass::ApplyRenderSettings(Settings);
}

bool ForwardShadingPass::BatchDrawable(const SceneNode* Object)
{
	if (Object)
	{
		if (Object->Type() == SceneNode::EType::Model)
		{
			auto Model = static_cast<const ModelInstance*>(Object);
			assert(Model);

			if (!Model->Visible())
			{
				return false;
			}

			auto TempModel = Model->GetModel();
			if (!TempModel)
			{
				return false;
			}

			if (m_EnableFrustumCulling)
			{
				if (!m_Frustum.IntersectsWith(TempModel->BoundingBox()))
				{
					return false;
				}
			}

			for (auto& Mesh : TempModel->Meshes())
			{
				if (!Mesh.Visible() || !Mesh.GetMaterial())
				{
					continue;
				}

				auto TempMesh = Mesh.GetMesh();
				if (!TempMesh)
				{
					continue;
				}

				if (m_EnableFrustumCulling)
				{
					if (!m_Frustum.IntersectsWith(TempMesh->BoundingBox()))
					{
						continue;
					}
				}

				auto Find = std::find_if(m_Drawables.begin(), m_Drawables.end(), [&, Mesh](const Drawable& DstDrawable) {
					return DstDrawable.Mesh == &Mesh;
				});
				if (Find == m_Drawables.end())
				{
					m_Drawables.emplace_back(std::move(
						Drawable
						{
							&Mesh,
							std::vector<const SceneNode*>{Object}
						}));
				}
				else
				{
					Find->m_Instances.push_back(Object);
				}
			}

			return true;
		}
	}

	return true;
}

void ForwardShadingPass::Render(ICommandBuffer* Command)
{
	assert(Command);

	SCOPED_STATS(ForwardOpaquePass);

	SCOPED_RENDER_EVENT(Command, ForwardOpaquePass, Color::Random());

	Command->ClearColorImage(m_FrameBuffer->Description().ColorAttachments[0].get(), Color::DarkBlue);

	Command->ClearDepthStencilImage(m_FrameBuffer->Description().DepthStencilAttachment.get());

	m_Scene->Traverse(std::bind(&ForwardShadingPass::BatchDrawable, this, std::placeholders::_1));

	for (auto& Drawable : m_Drawables)
	{
		Draw(Command, Drawable);
	}

	m_Drawables.clear(); /// #TODO
}

NAMESPACE_END(RHI)

#endif

class OpaquePassMeshDrawCommandBuilder : public GeometryPassMeshDrawCommandBuilder<GenericVS, DefaultLit>
{
public:
	using GeometryPassMeshDrawCommandBuilder::GeometryPassMeshDrawCommandBuilder;

	MeshDrawCommand Build(const StaticMesh& Mesh, const Scene&) override final
	{
		return MeshDrawCommand(Mesh);
	}
};

OpaquePass::OpaquePass(DAGNodeID ID, RenderGraph& Graph)
	: GeometryPass(ID, "OpaquePass", Graph, EGeometryPassFilter::Opaque, new OpaquePassMeshDrawCommandBuilder(Graph.GetGraphicsSettings()))
{
}
