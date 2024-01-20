#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPassField.h"

class ResourceManager
{
public:
	ResourceManager(class RHIDevice& Device, DirectedAcyclicGraph& Graph);

	~ResourceManager();

	RDGResource& GetOrAllocateResource(const char* Name, RDGResource::EVisibility Visibility);

	void CreateResources();
private:
	std::unordered_map<std::string_view, std::shared_ptr<RDGResource>> m_Resources;
	class RHIDevice& m_RHIDevice;
	DirectedAcyclicGraph& m_Graph;
};