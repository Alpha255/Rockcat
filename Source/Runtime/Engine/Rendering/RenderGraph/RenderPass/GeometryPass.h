#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass.h"

struct MeshDrawCommandBuilder
{
	virtual std::shared_ptr<MeshDrawCommand> Build(const class StaticMesh& Mesh, const struct RenderSettings& GraphicsSettings) = 0;
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