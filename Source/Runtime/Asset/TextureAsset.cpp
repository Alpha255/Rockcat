#include "Asset/TextureAsset.h"
#include "RHI/RHIDevice.h"

void TextureAsset::CreateRHI(RHIDevice& Device, const RHITextureCreateInfo& CreateInfo)
{
	if (m_Texture)
	{
		return;
	}

	m_Texture = Device.CreateTexture(CreateInfo);
}
