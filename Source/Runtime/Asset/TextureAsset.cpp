#include "Asset/TextureAsset.h"
#include "RHI/RHIDevice.h"

void TextureAsset::CreateRHI(RHIDevice& Device, const RHITextureDesc& Desc)
{
	if (m_Texture)
	{
		return;
	}

	m_Texture = Device.CreateTexture(Desc);
}
