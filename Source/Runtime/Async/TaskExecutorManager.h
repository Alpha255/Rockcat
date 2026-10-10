#pragma once

#include "Core/Singleton.h"
#include "Async/TaskEvent.h"

namespace tf
{
	class Executor;
};

class TFTaskExecutorManager : public Singleton<TFTaskExecutorManager>
{
public:
	void Initialize();
	void Finalize();

	uint32_t GetNumWorkerThreads();

	tf::Executor* GetExecutor(ETFTaskThread Thread, ETFTaskPriority Priority);
private:
	void InitializeThreadTags();

	std::vector<std::shared_ptr<tf::Executor>> m_Executors;
};