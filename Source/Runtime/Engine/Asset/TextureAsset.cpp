#include "Engine/Asset/TextureAsset.h"
#include "Engine/RHI/RHIDevice.h"
#include "Engine/RHI/RHICommandListContext.h"

void TextureAsset::CreateRHI(RHIDevice& Device, RHICommandListContext* CommandListContext)
{
	if (m_Texture)
	{
		return;
	}

	assert(CommandListContext);

	m_Texture = Device.CreateTexture(m_CreateInfo, CommandListContext->GetCommandBuffer());
	m_CreateInfo.InitialData.Data.reset();
}
