#include "Rendering/RenderScene.h"
#include "Scene/Scene.h"
#include "Scene/Components/StaticMesh.h"
#include "Scene/SceneVisitor.h"
#include "RHI/RHIDevice.h"
#include "Rendering/RenderGraph/RenderPass/GeometryPass.h"

MeshDrawCommand::MeshDrawCommand(const StaticMesh& Mesh)
	: IndexBuffer(Mesh.GetIndexBuffer())
	//, Material(&Mesh.GetMaterialProperty())
	, FirstIndex(0u)
	, NumPrimitives(Mesh.GetNumPrimitive())
	, NumIndex(Mesh.GetNumIndex())
	, IndexFormat(Mesh.GetIndexFormat())
{
	VertexArgs.BaseIndex = 0;
	VertexArgs.NumVertex = Mesh.GetNumVertex();
}

RenderScene::RenderScene(const Scene& InScene)
	: m_Scene(InScene)
{
}
