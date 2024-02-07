#include "Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Engine/RHI/RHIDevice.h"
#include "Engine/RHI/RHIInterface.h"

ResourceManager::ResourceManager(RHIInterface& RHI, DirectedAcyclicGraph& Graph)
	: m_RHI(RHI)
	, m_Graph(Graph)
{
}

ResourceManager::~ResourceManager()
{
}

RDGResource& ResourceManager::GetOrAllocateResource(const char* Name, RDGResource::EVisibility Visibility)
{
	auto It = m_Resources.find(Name);
	if (It != m_Resources.end())
	{
		It->second->SetVisibility(Visibility);
		return *It->second;
	}

	auto NewResource = std::make_shared<RDGResource>(m_Graph.AddNode(), Name, Visibility);
	m_Resources.insert(std::make_pair(std::string_view(Name), NewResource));
	return *NewResource;
}

RHIGraphicsPipelinePtr ResourceManager::GetOrCreateGraphicsPipeline(const RHIGraphicsPipelineCreateInfo& CreateInfo)
{
	return RHIGraphicsPipelinePtr();
}

void ResourceManager::CreateAllResources()
{
	for (auto& [Name, Resource] : m_Resources)
	{
		switch (Resource->GetType())
		{
		case RDGResource::EResourceType::Buffer:
		{
			auto& CreateInfo = Resource->GetBufferCreateInfo();
		}
			break;
		case RDGResource::EResourceType::Image:
		{
			auto& CreateInfo = Resource->GetImageCreateInfo();
		}
			break;
		}
	}
}

//void ResourceManager::AllocateSceneImageFields()
//{
//	m_SceneImages.Depth = GetOrAllocateField(
//		"SceneDepth", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();
//
//	m_SceneImages.Color = GetOrAllocateField(
//		"SceneColor", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();
//
//	m_SceneImages.ShadowMap = GetOrAllocateField(
//		"SceneShadowMap", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();
//
//	auto RenderingPath = RHIInterface::GetGraphicsSettings().RenderingPath;
//	if (RenderingPath == ERenderingPath::DeferredShading || RenderingPath == ERenderingPath::DeferredLighting)
//	{
//		m_SceneImages.GBuffer.Albedo = GetOrAllocateField(
//			"GBuffer.Albedo", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();
//		m_SceneImages.GBuffer.WorldNormal = GetOrAllocateField(
//			"GBuffer.WorldNormal", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();
//		m_SceneImages.GBuffer.MetallicRoughness = GetOrAllocateField(
//			"GBuffer.MetallicRoughness", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();
//	}
//}
