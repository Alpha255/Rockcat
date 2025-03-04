#pragma once

#include "Engine/RHI/RHICommandBuffer.h"

class RHICommandListContext
{
public:
	RHICommandBuffer* GetCommandBuffer(ERHICommandBufferLevel Level = ERHICommandBufferLevel::Primary)
	{
		auto& CommandBufferList = Level == ERHICommandBufferLevel::Primary ? m_PrimaryCommandBufferList : m_SecondaryCommandBufferList;

		if (CommandBufferList.Active && CommandBufferList.Active->IsRecording())
		{
			return CommandBufferList.Active;
		}

		CommandBufferList.Active = nullptr;

		for (auto& CommandBuffer : CommandBufferList)
		{
			CommandBuffer->RefreshStatus();
			RHICommandBuffer*& Active = CommandBufferList.Active;

			if (CommandBuffer->IsRecording())
			{
				Active = CommandBuffer.get();
			}
			else if (CommandBuffer->IsReady())
			{
				CommandBuffer->Begin();
				Active = CommandBuffer.get();
			}
			else if (CommandBuffer->IsNeedReset())
			{
				CommandBuffer->Reset();
				CommandBuffer->Begin();
				Active = CommandBuffer.get();
			}

			if (Active)
			{
				return Active;
			}
		}

		CommandBufferList.Active = CommandBufferList.emplace_back(AllocateCommandBuffer(Level)).get();
		assert(CommandBufferList.Active);
		CommandBufferList.Active->Begin();
		return CommandBufferList.Active;
	}
	
	RHICommandBuffer* GetCommandBufferAsync(ERHICommandBufferLevel Level = ERHICommandBufferLevel::Primary)
	{
		auto& CommandBufferList = Level == ERHICommandBufferLevel::Primary ? m_PrimaryCommandBufferList : m_SecondaryCommandBufferList;
		
		std::lock_guard Locker(CommandBufferList.Lock);

		RHICommandBuffer* Ret = nullptr;
		if (CommandBufferList.Active && CommandBufferList.Active->IsRecording())
		{
			std::swap(Ret, CommandBufferList.Active);
			return Ret;
		}

		for (auto It = CommandBufferList.begin(); It != CommandBufferList.end(); ++It)
		{
			(*It)->RefreshStatus();

			if ((*It)->IsRecording())
			{
				Ret = It->get();
			}
			else if ((*It)->IsReady())
			{
				(*It)->Begin();
				Ret = It->get();
			}
			else if ((*It)->IsNeedReset())
			{
				(*It)->Reset();
				(*It)->Begin();
				Ret = It->get();
			}

			if (Ret)
			{
				CommandBufferList.OnTheFlyAsync.push_back(*It);
				CommandBufferList.erase(It);
				return Ret;
			}
		}

		Ret = CommandBufferList.OnTheFlyAsync.emplace_back(AllocateCommandBuffer(Level)).get();
		assert(Ret);
		Ret->Begin();
		return Ret;
	}

	virtual void SubmitActiveCommandBuffer() = 0;
protected:
	virtual RHICommandBufferPtr AllocateCommandBuffer(ERHICommandBufferLevel Level) = 0;

	struct RHICommandBufferList : public std::list<RHICommandBufferPtr>
	{
		std::mutex Lock;
		RHICommandBuffer* Active = nullptr;
		std::vector<RHICommandBufferPtr> OnTheFlyAsync;
	};

	RHICommandBufferList m_PrimaryCommandBufferList;
	RHICommandBufferList m_SecondaryCommandBufferList;
};