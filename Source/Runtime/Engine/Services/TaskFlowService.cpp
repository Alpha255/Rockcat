#include "Engine/Services/TaskFlowService.h"
#include "Engine/Services/SpdLogService.h"
#include "Core/PlatformMisc.h"

#define FILE_WATCHER_THREADS 2
#define RENDER_THREADS 1

TaskFlowService::TaskFlowService()
	: m_UseHyperThreading(false)
{
	size_t NumSeperateThread = 0u;
	auto NumTotalThreads = PlatformMisc::GetHardwareConcurrencyThreadsCount(m_UseHyperThreading);

	m_Executors[(size_t)EThread::GameThread] = std::move(std::make_unique<tf::Executor>(1u));
	NumSeperateThread += 1u;

	m_Executors[(size_t)EThread::RenderThread] = std::move(std::make_unique<tf::Executor>(1u));
	NumSeperateThread += 1u;

	m_Executors[(size_t)EThread::FileWatchThread] = std::move(std::make_unique<tf::Executor>(1u));
	NumSeperateThread += 1u;

	m_Executors[(size_t)EThread::WorkerThread] = std::move(std::make_unique<tf::Executor>(NumTotalThreads - NumSeperateThread));

	LOG_INFO("TaskFlow: Create taskflow executors with {} threads, {} seperate threads, {} work threads, hyper threading is {}, taskflow version: {}",
		NumTotalThreads,
		NumSeperateThread,
		NumTotalThreads - NumSeperateThread,
		m_UseHyperThreading ? "enabled" : "disabled",
		tf::version());
}

void TaskFlowService::OnStartup()
{
}

void TaskFlowService::OnShutdown()
{
}
