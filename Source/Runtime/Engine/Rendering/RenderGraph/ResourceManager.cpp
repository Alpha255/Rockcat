#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/RHI/RHIDevice.h"
#include "Engine/Services/RenderService.h"

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

	auto Resource = std::make_shared<RDGResource>(m_ResourceIDAllocator.Allocate(), Type, Name, Visibility);
	m_Resources.insert(std::make_pair(std::string_view(Name), Resource));
	return *Resource;
}

void ResourceManager::OnWindowResized(uint32_t, uint32_t)
{
}

void ResourceManager::ResolveResources(RHIDevice& Device)
{
	for (auto& [Name, Resource] : m_Resources)
	{
		Resource->CreateRHI(Device);
	}
}
