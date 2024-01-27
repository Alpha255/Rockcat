#include "Runtime/Engine/Rendering/RenderGraph/RenderPassField.h"

RDGResource& RDGResource::SetVisibility(EVisibility Visibility)
{
	m_Visibility = (Visibility | m_Visibility);
	return *this;
}

RHIImageCreateInfo& RDGResource::CreateAsImage()
{
	assert(!m_ResourceCreateInfo);
	m_ResourceCreateInfo = std::make_optional(RHIImageCreateInfo());
	return std::get<RHIImageCreateInfo>(m_ResourceCreateInfo.value()).SetName(m_Name.data());
}

RHIBufferCreateInfo& RDGResource::CreateAsBuffer()
{
	assert(!m_ResourceCreateInfo);
	m_ResourceCreateInfo = std::make_optional(RHIBufferCreateInfo());
	return std::get<RHIBufferCreateInfo>(m_ResourceCreateInfo.value()).SetName(m_Name.data());
}
