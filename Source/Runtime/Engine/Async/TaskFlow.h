#pragma once

#include "Core/PlatformMisc.h"

namespace tf
{
	template<class TTask>
	class ThreadTaskFlow : public tf::Taskflow
	{
	public:
		using tf::Taskflow::Taskflow;

		template<class... Arg>
		TTask& Emplace(Arg&&... Args)
		{
			TTask* NewTask = m_Tasks.emplace_back(std::make_shared<TTask>(std::forward<Arg>(Args)...)).get();
			new (NewTask) TTask(tf::Taskflow::emplace([NewTask]() {
				PlatformMisc::ThreadPriorityGuard PriorityGuard(std::this_thread::get_id(), NewTask->GetPriority());
				NewTask->Execute();
				}));
			return *NewTask;
		}
	private:
		std::vector<std::shared_ptr<TTask>> m_Tasks;
	};
}
