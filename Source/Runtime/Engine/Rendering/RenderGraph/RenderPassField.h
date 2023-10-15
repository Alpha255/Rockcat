#pragma once

#include "Runtime/Core/DirectedAcyclicGraph.h"
#include "Runtime/Engine/RHI/RHIBuffer.h"

using DAGNodeID = DirectedAcyclicGraph::NodeID;

class Field
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

	Field(DAGNodeID ID, const char8_t* Name, EVisibility Visibility, EResourceType Type)
		: m_Visibility(Visibility)
		, m_ResourceType(Type)
		, m_NodeID(ID)
		, m_Name(Name)
	{
		assert(Name);
	}

	Field& SetName(const char8_t* Name) 
	{ 
		if (Name && m_Name != Name)
		{
			m_Name = Name;
		}
		return *this; 
	}
	const char8_t* GetName() const { return m_Name.data(); }

	EVisibility GetVisibility() const { return m_Visibility; }
	Field& SetVisibility(EVisibility Visibility);

	EResourceType GetResourceType() const { return m_ResourceType; }

	DAGNodeID GetNodeID() const { return m_NodeID; }

	RHIBufferCreateInfo& GetBufferCreateInfo() 
	{ 
		assert(m_ResourceType == EResourceType::Buffer); 
		return std::get<RHIBufferCreateInfo>(m_ResourceCreateInfo);
	}

	RHIImageCreateInfo& GetImageCreateInfo() 
	{ 
		assert(m_ResourceType == EResourceType::Image);
		return std::get<RHIImageCreateInfo>(m_ResourceCreateInfo);
	}

	template<class Archive>
	void serialize(Archive& Ar)
	{
		Ar(
			CEREAL_NVP(m_Visibility),
			CEREAL_NVP(m_ResourceType),
			//CEREAL_NVP(m_Name),
			CEREAL_NVP(m_NodeID),
			CEREAL_NVP(m_ResourceCreateInfo)
		);
	}
protected:
private:
	EVisibility m_Visibility = EVisibility::None;
	EResourceType m_ResourceType = EResourceType::Image;
	DAGNodeID m_NodeID;
	std::string_view m_Name;

	std::variant<RHIBufferCreateInfo, RHIImageCreateInfo> m_ResourceCreateInfo;
};

ENUM_FLAG_OPERATORS(Field::EVisibility)

struct RenderPassField
{
	DAGNodeID ID;
	Field::EVisibility Visibility;
};
