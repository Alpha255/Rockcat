#pragma once

#include "Engine/RHI/RHIInterface.h"
#include "Engine/Rendering/RenderGraph/RenderPassField.h"
#include "Engine/RHI/RHIPipeline.h"

class ResourceManager
{
public:
	ResourceManager(const GraphicsSettings& GfxSettings, DirectedAcyclicGraph& Graph);

	~ResourceManager();

	RDGResource& GetOrAllocateResource(const char* Name, RDGResource::EVisibility Visibility);

	RHIGraphicsPipelinePtr GetOrCreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& CreateInfo);

	void ResolveResources();
private:
	std::unordered_map<std::string_view, std::shared_ptr<RDGResource>> m_Resources;
	std::unordered_map<size_t, RHIBufferPtr> m_FreeBuffers;
	std::unordered_map<size_t, RHITexturePtr> m_FreeTextures;
	class RHIDevice& m_RHIDevice;
	DirectedAcyclicGraph& m_Graph;
};