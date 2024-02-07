#include "Engine/Services/TaskFlowService.h"
#include "Engine/Services/SpdLogService.h"
#include "Core/PlatformMisc.h"

#define FILE_WATCHER_THREADS 2
#define RENDER_THREADS 1

TaskFlowService::TaskFlowService()
	: m_UseHyperThreading(true)
{
	const size_t SeperateThreadCount = FILE_WATCHER_THREADS + RENDER_THREADS;
	auto ThreadsCount = PlatformMisc::GetHardwareConcurrencyThreadsCount(m_UseHyperThreading) - SeperateThreadCount;
	m_Executor = std::make_unique<tf::Executor>(ThreadsCount);

	LOG_INFO("TaskFlow: Create taskflow executor with {} threads, hyper threading is {}, taskflow version: {}",
		ThreadsCount,
		m_UseHyperThreading ? "enabled" : "disabled",
		tf::version());
}

void TaskFlowService::OnStartup()
{
}

void TaskFlowService::OnShutdown()
{
}
