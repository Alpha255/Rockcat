#include "Runtime/Engine/Rendering/RenderGraph/ResourceManager.h"
#include "Runtime/Engine/RHI/RHIDevice.h"
#include "Runtime/Engine/RHI/RHIInterface.h"

ResourceManager::ResourceManager(RHIDevice& RenderDevice, DirectedAcyclicGraph& Graph)
	: m_RenderDevice(RenderDevice)
	, m_Graph(Graph)
{
	AllocateSceneImageFields();
}

ResourceManager::~ResourceManager()
{
}

Field& ResourceManager::GetOrAllocateField(const char8_t* Name, Field::EVisibility Visibility, Field::EResourceType Type)
{
	auto It = m_Fields.find(Name);
	if (It != m_Fields.end())
	{
		It->second->SetName(Name)
			.SetVisibility(Visibility);
		return *It->second;
	}

	auto NewField = std::make_shared<Field>(m_Graph.AddNode(), Name, Visibility, Type);
	m_Fields.insert(std::make_pair(std::string_view(Name), NewField));
	return *NewField;
}

void ResourceManager::CreateAllResources()
{
}

void ResourceManager::AllocateSceneImageFields()
{
	m_SceneImages.Depth = GetOrAllocateField(
		"SceneDepth", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();

	m_SceneImages.Color = GetOrAllocateField(
		"SceneColor", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();

	m_SceneImages.ShadowMap = GetOrAllocateField(
		"SceneShadowMap", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();

	auto RenderingPath = RHIInterface::GetGraphicsSettings().RenderingPath;
	if (RenderingPath == ERenderingPath::DeferredShading || RenderingPath == ERenderingPath::DeferredLighting)
	{
		m_SceneImages.GBuffer.Albedo = GetOrAllocateField(
			"GBuffer.Albedo", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();
		m_SceneImages.GBuffer.WorldNormal = GetOrAllocateField(
			"GBuffer.WorldNormal", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();
		m_SceneImages.GBuffer.MetallicRoughness = GetOrAllocateField(
			"GBuffer.MetallicRoughness", Field::EVisibility::None, Field::EResourceType::Image).GetNodeID();
	}
}
