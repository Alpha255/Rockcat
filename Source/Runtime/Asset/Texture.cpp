#include "Asset/Texture.h"
#include "RHI/RHIDevice.h"

void Texture::CreateRHI(RHIDevice& Device, const RHITextureDesc& Desc)
{
	if (m_Texture)
	{
		return;
	}

	m_Texture = Device.CreateTexture(Desc);
}
