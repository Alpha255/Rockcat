#include "Runtime/Engine/Rendering/RenderGraph/RenderPassField.h"

RDGResource& RDGResource::SetVisibility(EVisibility Visibility)
{
	m_Visibility = (Visibility | m_Visibility);
	return *this;
}

RHIImageCreateInfo& RDGResource::CreateAsImage()
{
	assert(!m_ResourceCreateInfo);
	m_ResourceCreateInfo = std::make_optional(ResourceCreateInfo());
	return std::get<1u>(m_ResourceCreateInfo.value()).SetName(m_Name.data());
}

RHIBufferCreateInfo& RDGResource::CreateAsBuffer()
{
	assert(!m_ResourceCreateInfo);
	m_ResourceCreateInfo = std::make_optional(ResourceCreateInfo());
	return std::get<0u>(m_ResourceCreateInfo.value()).SetName(m_Name.data());
}
