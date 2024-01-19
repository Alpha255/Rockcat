#pragma once

#include "Runtime/Engine/Rendering/RenderGraph/RenderPassField.h"

class ResourceManager
{
public:
	struct SceneImages
	{
		struct GeometryBuffer
		{
			DAGNodeID Albedo;
			DAGNodeID WorldNormal;
			DAGNodeID MetallicRoughness;
		};

		DAGNodeID Depth;
		DAGNodeID Color;
		DAGNodeID ShadowMap;

		GeometryBuffer GBuffer;
	};

	ResourceManager(class RHIDevice& Device, DirectedAcyclicGraph& Graph);

	~ResourceManager();

	Field& GetOrAllocateField(const char* Name, Field::EVisibility Visibility, Field::EResourceType Type);

	const SceneImages& GetSceneImages() const { return m_SceneImages; }

	void CreateResources();
private:
	void AllocateSceneImageFields();

	std::unordered_map<std::string_view, std::shared_ptr<Field>> m_Fields;
	class RHIDevice& m_RHIDevice;
	DirectedAcyclicGraph& m_Graph;
	SceneImages m_SceneImages;
};