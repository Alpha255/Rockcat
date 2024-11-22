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
	IGeometryPassMeshDrawCommandBuilder(const GraphicsSettings& InGfxSettings)
		: GfxSettings(InGfxSettings)
	{
	}

	virtual MeshDrawCommand Build(const class StaticMesh& Mesh, const class Scene& InScene) = 0;

	const GraphicsSettings& GfxSettings;
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
	GeometryPass(DAGNodeID ID, const char* Name, class RenderGraph& Graph, EGeometryPassFilter Filter, IGeometryPassMeshDrawCommandBuilder* MeshDrawCommandBuilder);

	void Execute(class RHIDevice& Device, const RenderScene& Scene);

	EGeometryPassFilter GetGeometryPassFilter() const { return m_Filter; }
protected:
private:
	EGeometryPassFilter m_Filter;
};