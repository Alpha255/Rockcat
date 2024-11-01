#pragma once

#include "Engine/Rendering/RenderGraph/RenderPassField.h"
#include "Engine/RHI/RHIPipeline.h"

class ResourceManager
{
public:
	ResourceManager(class RHIInterface& RHI, DirectedAcyclicGraph& Graph);

	~ResourceManager();

	class RHIInterface& GetRHI() { return m_RHI; }

	RDGResource& GetOrAllocateResource(const char* Name, RDGResource::EVisibility Visibility);

	RHIGraphicsPipelinePtr GetOrCreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& CreateInfo);

	void CreateAllResources();
private:
	std::unordered_map<std::string_view, std::shared_ptr<RDGResource>> m_Resources;
	std::unordered_map<size_t, RHIBufferPtr> m_FreeBuffers;
	std::unordered_map<size_t, RHITexturePtr> m_FreeTextures;
	class RHIInterface& m_RHI;
	DirectedAcyclicGraph& m_Graph;
};