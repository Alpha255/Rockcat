#include "Engine/Asset/TextureAsset.h"
#include "Engine/RHI/RHIDevice.h"

void TextureAsset::CreateRHI(RHIDevice& Device)
{
	if (m_Texture)
	{
		return;
	}

	m_Texture = Device.CreateTexture(m_CreateInfo);
	
	m_CreateInfo.InitialData.Data.reset();
}
