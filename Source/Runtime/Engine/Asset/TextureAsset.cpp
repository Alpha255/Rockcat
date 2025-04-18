#include "Engine/Asset/TextureAsset.h"
#include "Engine/RHI/RHIDevice.h"
#include "Engine/RHI/RHICommandListContext.h"

void TextureAsset::CreateRHI(RHIDevice& Device, const RHITextureCreateInfo& CreateInfo)
{
	if (m_Texture)
	{
		return;
	}

	m_Texture = Device.CreateTexture(CreateInfo);
}
