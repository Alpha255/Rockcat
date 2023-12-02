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

	ResourceManager(class RHIDevice& RenderDevice, DirectedAcyclicGraph& Graph);

	Field& GetOrAllocateField(const char8_t* Name, Field::EVisibility Visibility, Field::EResourceType Type,
		DAGNodeID RefID = DAGNodeID());

	const SceneImages& GetSceneImages() const { return m_SceneImages; }

	void CreateAllResources();
private:
	void AllocateSceneImageFields();

	std::unordered_map<DAGNodeID, std::shared_ptr<Field>> m_Fields;
	class RHIDevice& m_RenderDevice;
	DirectedAcyclicGraph& m_Graph;
	SceneImages m_SceneImages;
};