#pragma once

#include "Engine/Rendering/RenderGraph/RenderPass.h"

template<class TVertexShader, class TFragmentShader>
struct GeometryPassShaders
{
	TVertexShader VertexShader;
	TFragmentShader FragmentShader;
};

struct IMeshDrawCommandBuilder
{
	virtual MeshDrawCommand Build(const class StaticMesh& Mesh) = 0;
};

template<class VertexShader, class FragmentShader>
struct MeshDrawCommandBuilder : public IMeshDrawCommandBuilder
{
	GeometryPassShaders<VertexShader, FragmentShader> PassShaders;
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