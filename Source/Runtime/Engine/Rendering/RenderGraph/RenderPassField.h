#pragma once

#include "Runtime/Core/DirectedAcyclicGraph.h"
#include "Runtime/Engine/RHI/RHIBuffer.h"

using DAGNodeID = DirectedAcyclicGraph::NodeID;

class RDGResource
{
public:
	enum class EVisibility
	{
		None = 0x0,
		Input = 0x1,
		Output = 0x2,
		Internal = 0x4
	};

	enum class EResourceType
	{
		Image,
		Buffer
	};

	RDGResource(DAGNodeID ID, const char* Name, EVisibility Visibility)
		: m_Visibility(Visibility)
		, m_NodeID(ID)
		, m_Name(Name)
	{
	}

	RDGResource& SetName(const char* Name) { m_Name = Name; return *this; }
	const char* GetName() const { return m_Name.data(); }

	EVisibility GetVisibility() const { return m_Visibility; }
	RDGResource& SetVisibility(EVisibility Visibility);

	EResourceType GetType() const { return m_Type; }

	DAGNodeID GetNodeID() const { return m_NodeID; }

	RHIImageCreateInfo& CreateAsImage();
	RHIBufferCreateInfo& CreateAsBuffer();

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Visibility),
			CEREAL_NVP(m_Type),
			//CEREAL_NVP(m_Name),
			CEREAL_NVP(m_NodeID),
			CEREAL_NVP(m_ResourceCreateInfo)
		);
	}
protected:
	using ResourceCreateInfo = std::variant<RHIBufferCreateInfo, RHIImageCreateInfo>;
private:
	EVisibility m_Visibility = EVisibility::None;
	EResourceType m_Type = EResourceType::Image;
	DAGNodeID m_NodeID;
	std::string_view m_Name;

	std::optional<ResourceCreateInfo> m_ResourceCreateInfo;
};

ENUM_FLAG_OPERATORS(RDGResource::EVisibility)

struct RenderPassField
{
	DAGNodeID ResourceID;
	RDGResource::EVisibility Visibility;
};
