#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/ShadowPass.h"
#include "Runtime/Engine/RHI/RHIInterface.h"

const char8_t* GetShadowTechniqueName(EShadowTechnique Technique)
{
	switch (Technique)
	{
	case EShadowTechnique::Generic:
		return "GenericShadowMapPass";
	case EShadowTechnique::Cascade:
		return "CascadeShadowMapPass";
	default:
		return "None";
	}
}

ShadowPass::ShadowPass(DAGNodeID ID, ResourceManager& ResourceMgr)
	: MeshPass(
		ID, 
		GetShadowTechniqueName(RHIInterface::GetGraphicsSettings().ShadowTechnique),
		ResourceMgr,
		EMeshPass::ShadowCaster)
{
}
