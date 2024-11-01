#include "Engine/Rendering/RenderGraph/RenderPassField.h"

RDGResource& RDGResource::SetVisibility(EVisibility Visibility)
{
	m_Visibility = (Visibility | m_Visibility);
	return *this;
}

RHITextureCreateInfo& RDGResource::CreateAsTexture()
{
	assert(!m_ResourceCreateInfo);
	m_ResourceCreateInfo = std::make_optional(RHITextureCreateInfo());
	return std::get<RHITextureCreateInfo>(m_ResourceCreateInfo.value()).SetName(m_Name.data());
}

RHIBufferCreateInfo& RDGResource::CreateAsBuffer()
{
	assert(!m_ResourceCreateInfo);
	m_ResourceCreateInfo = std::make_optional(RHIBufferCreateInfo());
	return std::get<RHIBufferCreateInfo>(m_ResourceCreateInfo.value()).SetName(m_Name.data());
}
