#pragma once

#include "Core/Math/Transform.h"
#include "Engine/Rendering/RenderGraph/RenderPass.h"

template<class TVertexShader, class TFragmentShader>
struct GeometryPassShaders
{
	TVertexShader VertexShader;
	TFragmentShader FragmentShader;
};

struct IGeometryPassMeshDrawCommandBuilder
{
	IGeometryPassMeshDrawCommandBuilder(class RHIInterface& InBackend)
		: Backend(InBackend)
	{
	}

	virtual MeshDrawCommand Build(const class StaticMesh& Mesh, const class Scene& InScene) = 0;

	class RHIInterface& Backend;
};

template<class TVertexShader, class TFragmentShader>
struct GeometryPassMeshDrawCommandBuilder : public IGeometryPassMeshDrawCommandBuilder
{
	using IGeometryPassMeshDrawCommandBuilder::IGeometryPassMeshDrawCommandBuilder;
	GeometryPassShaders<TVertexShader, TFragmentShader> PassShader;
};

class GeometryPass : public RenderPass
{
public:
	GeometryPass(
		DAGNodeID ID, 
		const char* Name, 
		class RenderGraph& Graph, 
		EGeometryPassFilter Filter, 
		IGeometryPassMeshDrawCommandBuilder* MeshDrawCommandBuilder);

	void Execute(const RenderScene& Scene) override;

	EGeometryPassFilter GetGeometryPassFilter() const { return m_Filter; }
protected:
	virtual RHIFrameBuffer* GetFrameBuffer();
private:
	EGeometryPassFilter m_Filter;
	RHIFrameBuffer* m_FrameBuffer = nullptr;
};