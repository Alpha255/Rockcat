#include "Runtime/Engine/Rendering/RenderGraph/RenderPass/ShadowPass.h"
#include "Runtime/Engine/RHI/RHIInterface.h"

const char8_t* GetNameByShadowTechnique(EShadowTechnique Technique)
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
		GetNameByShadowTechnique(RHIInterface::GetGraphicsSettings()->ShadowTechnique), 
		ResourceMgr)
{
}
