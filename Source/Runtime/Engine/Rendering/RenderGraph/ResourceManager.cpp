#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/RHI/RHIDevice.h"

ResourceManager::~ResourceManager()
{
}

RDGResource& ResourceManager::GetOrAllocateResource(RDGResource::EType Type, const char* Name, RDGResource::EVisibility Visibility)
{
	auto It = m_Resources.find(Name);
	if (It != m_Resources.end())
	{
		assert(Type == It->second->GetType());
		It->second->SetVisibility(Visibility);
		return *It->second;
	}

	auto Resource = std::make_shared<RDGResource>(RDGResourceID(m_NextResourceID++), Type, Name, Visibility);
	m_Resources.insert(std::make_pair(std::string_view(Name), Resource));
	return *Resource;
}

void ResourceManager::ResolveResources(RHIDevice& Device)
{
	for (auto& [Name, Resource] : m_Resources)
	{
		Resource->CreateRHI(Device);
	}
}
