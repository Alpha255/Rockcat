#pragma once

#include "Engine/RHI/RHICommandBuffer.h"

class RHICommandListContext
{
public:
	RHICommandBuffer* GetCommandBuffer(ERHICommandBufferLevel Level = ERHICommandBufferLevel::Primary)
	{
		auto& CmdBufferGroup = Level == ERHICommandBufferLevel::Primary ? m_PrimaryCmdBufferGroup : m_SecondaryCmdBufferGroup;
		if (auto ActiveCmdBuffer = CmdBufferGroup.ActiveCmdBuffer)
		{
			if (ActiveCmdBuffer->IsRecording())
			{
				return ActiveCmdBuffer;
			}
			else if (ActiveCmdBuffer->IsReady())
			{
				ActiveCmdBuffer->Begin();
				return ActiveCmdBuffer;
			}
			else if (ActiveCmdBuffer->IsNeedReset())
			{
				ActiveCmdBuffer->Reset();
				ActiveCmdBuffer->Begin();
				return ActiveCmdBuffer;
			}
		}

		CmdBufferGroup.ActiveCmdBuffer = CmdBufferGroup.OnFlyCmdBuffers.emplace_back(AllocateCommandBuffer(Level)).get();
		assert(CmdBufferGroup.ActiveCmdBuffer);

		CmdBufferGroup.ActiveCmdBuffer->Begin();
		return CmdBufferGroup.ActiveCmdBuffer;
	}

	RHICommandBuffer* GetUploadCommandBuffer() { return nullptr; }

	virtual void Submit() = 0;
	virtual void SubmitUploadCommandBuffer() = 0;
protected:
	virtual RHICommandBufferPtr AllocateCommandBuffer(ERHICommandBufferLevel Level) = 0;

	struct CommandBufferGroup
	{
		std::vector<RHICommandBufferPtr> OnFlyCmdBuffers;
		std::queue<RHICommandBufferPtr> FreedCmdBuffers;
		RHICommandBuffer* ActiveCmdBuffer = nullptr;
	};

	CommandBufferGroup m_PrimaryCmdBufferGroup;
	CommandBufferGroup m_SecondaryCmdBufferGroup;
};