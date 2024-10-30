#pragma once

#include "Engine/RHI/RHICommandBuffer.h"

class RHICommandListContext
{
public:
	RHICommandListContext()
		: m_ImmediateCmdBuffer(GetCommandBuffer())
	{
		assert(m_ImmediateCmdBuffer);
		m_ImmediateCmdBuffer->Begin();
	}

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
	}

	virtual void Submit() = 0;
protected:
	std::vector<RHICommandBufferPtr> m_CmdBuffersInFly;
	std::queue<RHICommandBufferPtr> m_CmdBuffersFreed;

	RHICommandBuffer* m_ImmediateCmdBuffer = nullptr;
	RHICommandBuffer* m_ActiveCmdBuffer = nullptr;
	RHICommandBuffer* m_UploadCmdBuffer = nullptr;
};