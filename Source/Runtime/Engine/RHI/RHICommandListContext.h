#pragma once

#include "Engine/RHI/RHICommandBuffer.h"

class RHICommandListContext
{
public:
	RHICommandBuffer* GetCommandBuffer(ERHICommandBufferLevel Level = ERHICommandBufferLevel::Immediate, bool UseForUploadOnly = false)
	{
		if (m_ActiveCmdBuffer && m_ActiveCmdBuffer->GetLevel() == Level)
		{
			if (m_ActiveCmdBuffer->IsReady())
			{
				return m_ActiveCmdBuffer;
			}
			if (m_ActiveCmdBuffer->IsNeedReset())
			{
				m_ActiveCmdBuffer->Reset();
				m_ActiveCmdBuffer->Begin();
				return m_ActiveCmdBuffer;
			}
		}

		m_ActiveCmdBuffer = m_CmdBuffersInFly.emplace_back(AllocateCommandBuffer(Level)).get();
		assert(m_ActiveCmdBuffer);

		m_ActiveCmdBuffer->Begin();

		return m_ActiveCmdBuffer;
	}

	virtual RHICommandBufferPtr AllocateCommandBuffer(ERHICommandBufferLevel Level) = 0;
	virtual void Submit() = 0;
protected:
	std::vector<RHICommandBufferPtr> m_CmdBuffersInFly;
	std::queue<RHICommandBufferPtr> m_CmdBuffersFreed;

	RHICommandBuffer* m_ActiveCmdBuffer = nullptr;
	RHICommandBuffer* m_UploadCmdBuffer = nullptr;
};