#pragma once

#include "Rendering/RenderSettings.h"
#include "Rendering/RenderGraph/RenderPassField.h"

class ResourceManager
{
public:
	ResourceManager(class RHIDevice& Device);
	~ResourceManager();

	void ResolveResources();

	RDGResource& GetOrAllocateResource(RDGResource::EType Type, const char* Name, RDGResource::EVisibility Visibility);
	const RDGResource& GetResource(RDGResourceID ID);
private:
	std::unordered_map<std::string_view, std::shared_ptr<RDGResource>> m_Resources;
	RDGResourceID::IndexType m_NextResourceID = 0u;
	class RHIDevice& m_Device;
};