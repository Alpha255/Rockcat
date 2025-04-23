#pragma once

#include "Core/DirectedAcyclicGraph.h"
#include "RHI/RHIBuffer.h"

using DAGNodeID = DirectedAcyclicGraph::NodeID;
using RDGResourceID = ObjectID<class RDGResource>;

class RDGResource
{
public:
	enum class EVisibility
	{
		None = 0x0,
		Input = 0x1,
		Output = 0x2,
		Internal = 0x4,
		External = 0x8
	};

	enum class EType
	{
		Buffer,
		Texture,
	};

	RDGResource(RDGResourceID ID, EType Type, const char* Name, EVisibility Visibility);

	RDGResourceID GetID() const { return m_ID; }

	RDGResource& SetName(const char* Name) { m_Name = Name; return *this; }
	const char* GetName() const { return m_Name.data(); }

	EVisibility GetVisibility() const { return m_Visibility; }
	RDGResource& SetVisibility(EVisibility Visibility);

	EType GetType() const { return m_Type; }

	void CreateRHI(class RHIDevice& Device);

	template<class T>
	inline T* GetRHI() const { return Cast<T>(m_RHIResource.get()); }

	const RHITextureCreateInfo& GetTextureCreateInfo() const
	{
		assert(m_Type == EType::Texture && m_ResourceCreateInfo.has_value());
		return std::get<RHITextureCreateInfo>(m_ResourceCreateInfo.value());
	}
	RHITextureCreateInfo& GetTextureCreateInfo()
	{
		assert(m_Type == EType::Texture && m_ResourceCreateInfo.has_value());
		return std::get<RHITextureCreateInfo>(m_ResourceCreateInfo.value());
	}

	const RHIBufferCreateInfo& GetBufferCreateInfo() const
	{
		assert(m_Type == EType::Buffer && m_ResourceCreateInfo.has_value());
		return std::get<RHIBufferCreateInfo>(m_ResourceCreateInfo.value());
	}
	RHIBufferCreateInfo& GetBufferCreateInfo()
	{
		assert(m_Type == EType::Buffer && m_ResourceCreateInfo.has_value());
		return std::get<RHIBufferCreateInfo>(m_ResourceCreateInfo.value());
	}
protected:
	using ResourceCreateInfo = std::variant<RHIBufferCreateInfo, RHITextureCreateInfo>;
private:
	void CreateAsTexture();
	void CreateAsBuffer();

	inline void SetType(EType Type) { m_Type = Type; }

	RDGResourceID m_ID;
	EVisibility m_Visibility = EVisibility::None;
	EType m_Type = EType::Texture;
	std::string_view m_Name;

	RHIResourcePtr m_RHIResource;
	std::optional<ResourceCreateInfo> m_ResourceCreateInfo;
};

ENUM_FLAG_OPERATORS(RDGResource::EVisibility)

struct RenderPassField
{
	RDGResourceID ResourceID;
	RDGResource::EVisibility Visibility;
};
