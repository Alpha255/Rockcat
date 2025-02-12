#pragma once

#include "Engine/Application/GraphicsSettings.h"
#include "Engine/Rendering/RenderGraph/RenderPassField.h"

class ResourceManager
{
public:
	~ResourceManager();

	void ResolveResources(class RHIDevice& Device);

	RDGResource& GetOrAllocateResource(RDGResource::EType Type, const char* Name, RDGResource::EVisibility Visibility);
	const RDGResource& GetResource(RDGResourceID ID);
private:
	std::unordered_map<std::string_view, std::shared_ptr<RDGResource>> m_Resources;
	RDGResourceID::IndexType m_NextResourceID = 0u;
};