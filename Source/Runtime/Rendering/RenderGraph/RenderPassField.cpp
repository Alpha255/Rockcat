#include "Rendering/RenderGraph/RenderPassField.h"
#include "RHI/RHIDevice.h"

RDGResource::RDGResource(RDGResourceID ID, EType Type, const char* Name, EVisibility Visibility)
	: m_ID(ID)
	, m_Visibility(Visibility)
	, m_Type(Type)
	, m_Name(Name)
{
	switch (m_Type)
	{
	case EType::Buffer:
		CreateAsBuffer();
		break;
	case EType::Texture:
		CreateAsTexture();
		break;
	}
}

RDGResource& RDGResource::SetVisibility(EVisibility Visibility) 
{
	m_Visibility = Visibility | m_Visibility; 
	return *this;
}

void RDGResource::CreateAsTexture()
{
	assert(!m_ResourceCreateInfo);
	SetType(EType::Texture);
	m_ResourceCreateInfo = std::make_optional(RHITextureCreateInfo());
	GetTextureCreateInfo().SetName(m_Name.data());
}

void RDGResource::CreateAsBuffer()
{
	assert(!m_ResourceCreateInfo);
	SetType(EType::Buffer);
	m_ResourceCreateInfo = std::make_optional(RHIBufferCreateInfo());
	GetBufferCreateInfo().SetName(m_Name.data());
}

void RDGResource::CreateRHI(RHIDevice& Device)
{
	assert(!m_RHIResource);

	switch (m_Type)
	{
	case EType::Buffer:
		m_RHIResource = Device.CreateBuffer(GetBufferCreateInfo());
		break;
	case EType::Texture:
		m_RHIResource = Device.CreateTexture(GetTextureCreateInfo());
		break;
	}
}
