#pragma once

#include "Engine/Application/GraphicsSettings.h"
#include "Engine/Rendering/RenderGraph/RenderPassField.h"

class ResourceManager
{
public:
	~ResourceManager();

	void ResolveResources(class RHIDevice& Device);

	RDGResource& GetOrAllocateResource(RDGResource::EType Type, const char* Name, RDGResource::EVisibility Visibility);

	void OnWindowResized(uint32_t /*Width*/, uint32_t /*Height*/);
private:
	std::unordered_map<std::string_view, std::shared_ptr<RDGResource>> m_Resources;

	RDGResourceIDAllocator m_ResourceIDAllocator;
};