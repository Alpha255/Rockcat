#include "Async/TaskExecutorManager.h"
#include "Async/Task.h"
#include "Core/ConsoleVariable.h"
#include "Misc/PlatformMisc.h"
#include "Core/SpdLogging.h"

#include <barrier>

ConsoleVariable<bool> CVarUseHyperThreading(
	"tf.use_hyper_threading",
	"Enable or disable hyper threading for taskflow executors.",
	false);

ConsoleVariable<bool> CVarUseSeperateGameThread(
	"tf.use_seperate_game_thread",
	"Enable or disable seperate game thread.",
	false);

ConsoleVariable<bool> CVarUseSeperateRenderThread(
	"tf.use_seperate_render_thread",
	"Enable or disable seperate render thread.",
	false);

ConsoleVariable<bool> CVarUseSeperateRHIThread(
	"tf.use_seperate_rhi_thread",
	"Enable or disable seperate rhi thread.",
	false);

ConsoleVariable<uint32_t> CVarNumForegroundThreads(
	"tf.num_foreground_threads",
	"Number of foreground threads.",
	2u);

thread_local ETFTaskThread t_ThreadTag = ETFTaskThread::WorkerThread;

void TFTaskExecutorManager::Initialize()
{
	LOG_INFO(LogTaskFlow, "Use taskflow @{}", tf::version());

	std::vector<size_t> WorkersInExecutor
	{
		CVarUseSeperateGameThread.Get(),
		CVarUseSeperateRenderThread.Get(),
		GetNumWorkerThreads(),
		CVarNumForegroundThreads.Get()
	};

	m_Executors.reserve(WorkersInExecutor.size());

	for (auto& NumWorkers : WorkersInExecutor)
	{
		if (NumWorkers > 0u)
		{
			m_Executors.emplace_back(std::make_shared<tf::Executor>(NumWorkers));
		}
	}

	if (const uint32_t NumForegroundThreads = CVarNumForegroundThreads.Get())
	{
		if (auto Executor = GetExecutor(ETFTaskThread::WorkerThread, ETFTaskPriority::High))
		{
			// A thread runs one task at a time, so blocking all N here forces the priority change above onto N distinct workers.
			std::barrier Barrier(static_cast<ptrdiff_t>(NumForegroundThreads));

			tf::Taskflow Flow;
			for (uint32_t Index = 0u; Index < NumForegroundThreads; ++Index)
			{
				Flow.emplace([&Barrier]() {
					PlatformMisc::SetThreadPriority(std::this_thread::get_id(), ETFTaskPriority::High);
					Barrier.arrive_and_wait();
					});
			}

			Executor->run(Flow);
			Executor->wait_for_all();

			LOG_INFO(LogTaskFlow, "Set {} foreground thread(s) to high priority", NumForegroundThreads);
		}
	}

	const uint32_t NumSeperateThreads = CVarUseSeperateGameThread.Get() + CVarUseSeperateRenderThread.Get() + CVarNumForegroundThreads.Get();
	LOG_INFO(LogTaskFlow, "Create executors with {} seperate threads, {} worker threads, hyper threading is {}",
		NumSeperateThreads,
		GetNumWorkerThreads(),
		CVarUseHyperThreading.Get() ? "enabled" : "disabled");
}

uint32_t TFTaskExecutorManager::GetNumWorkerThreads()
{
	const uint32_t NumTotalThreads = PlatformMisc::GetNumHardwareConcurrencyThreads(CVarUseHyperThreading.Get());

	uint32_t NumSeperateThreads = 0u;
	NumSeperateThreads += CVarUseSeperateGameThread.Get() ? 1u : 0u;
	NumSeperateThreads += CVarUseSeperateRenderThread.Get() ? 1u : 0u;
	NumSeperateThreads += CVarNumForegroundThreads.Get();

	return NumTotalThreads > NumSeperateThreads ? NumTotalThreads - NumSeperateThreads : 0u;
}

tf::Executor* TFTaskExecutorManager::GetExecutor(ETFTaskThread Thread, ETFTaskPriority Priority)
{
	assert(Thread < ETFTaskThread::Num);

	static const Array<size_t, ETFTaskThread> s_ExecutorIndices{
		static_cast<size_t>(ETFTaskThread::GameThread),
		static_cast<size_t>(ETFTaskThread::RenderThread) - !CVarUseSeperateGameThread.Get(),
		static_cast<size_t>(ETFTaskThread::WorkerThread) - !CVarUseSeperateGameThread.Get() - !CVarUseSeperateRenderThread.Get()
	};

	const size_t ThreadIndex = static_cast<size_t>(Thread);
	assert(ThreadIndex < s_ExecutorIndices.size());

	const bool IsHighPriority = (Priority > ETFTaskPriority::Normal) ||
		(Thread == ETFTaskThread::GameThread && !CVarUseSeperateGameThread.Get()) ||
		(Thread == ETFTaskThread::RenderThread && !CVarUseSeperateRenderThread.Get());

	const size_t ExecutorIndex = s_ExecutorIndices[ThreadIndex] + (IsHighPriority ? 1u : 0u);
	assert(ExecutorIndex < m_Executors.size());

	return m_Executors[ExecutorIndex].get();
}

void TFTaskExecutorManager::InitializeThreadTags()
{
	t_ThreadTag = ETFTaskThread::WorkerThread;

	if (CVarUseSeperateGameThread.Get())
	{
		TFTask::Launch("tf.SetGameThreadTag", []() {
			t_ThreadTag = ETFTaskThread::GameThread;
			}, ETFTaskThread::GameThread)->Wait();
	}

	if (CVarUseSeperateRenderThread.Get())
	{
		TFTask::Launch("tf.SetRenderThreadTag", []() {
			t_ThreadTag = ETFTaskThread::RenderThread;
			}, ETFTaskThread::RenderThread)->Wait();
	}
}

void TFTaskExecutorManager::Finalize()
{
	for (auto& Executor : m_Executors)
	{
		if (Executor)
		{
			Executor->wait_for_all();
			Executor.reset();
		}
	}

	m_Executors.clear();
}

bool TFTask::IsGameThread()
{
	return t_ThreadTag == ETFTaskThread::GameThread;
}

bool TFTask::IsRenderThread()
{
	return t_ThreadTag == ETFTaskThread::RenderThread;
}

bool TFTask::IsWorkerThread()
{
	return t_ThreadTag == ETFTaskThread::WorkerThread;
}