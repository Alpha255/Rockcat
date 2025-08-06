#pragma once

#include "Rendering/RenderGraph/RenderPass.h"

struct MeshDrawCommandBuilder
{
	virtual std::shared_ptr<MeshDrawCommand> Build(const class StaticMesh& Mesh, const class ISceneView& SceneView) = 0;

	void SetupShaderParameters(RHIGraphicsPipelineDesc& Desc,
		const class ISceneView& InView,
		const Math::Transform& InTransform,
		const MaterialProperty& InMaterial);
};

class GeometryPass : public RenderPass
{
public:
	using RenderPass::RenderPass;

	void Execute(const RenderScene& Scene) override;
protected:
	virtual RHIFrameBuffer* GetFrameBuffer();
private:
	RHIFrameBuffer* m_FrameBuffer = nullptr;
};