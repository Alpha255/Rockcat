#include "Engine/Services/TaskFlowService.h"
#include "Engine/Services/SpdLogService.h"
#include "Core/PlatformMisc.h"

TaskFlowService::TaskFlowService()
	: m_UseHyperThreading(false)
	, m_NumWorkThreads(0u)
{
}

void TaskFlowService::OnStartup()
{
	size_t NumSeperateThread = 0u;
	auto NumTotalThreads = PlatformMisc::GetHardwareConcurrencyThreadsCount(m_UseHyperThreading);

	m_Executors[(size_t)tf::EThread::GameThread] = std::move(std::make_unique<tf::Executor>(1u));
	NumSeperateThread += 1u;

	m_Executors[(size_t)tf::EThread::RenderThread] = std::move(std::make_unique<tf::Executor>(1u));
	NumSeperateThread += 1u;

	m_Executors[(size_t)tf::EThread::FileWatchThread] = std::move(std::make_unique<tf::Executor>(1u));
	NumSeperateThread += 1u;

	m_NumWorkThreads = static_cast<uint8_t>(NumTotalThreads - NumSeperateThread);
	m_Executors[(size_t)tf::EThread::WorkerThread] = std::move(std::make_unique<tf::Executor>(m_NumWorkThreads));

	LOG_INFO("TaskFlow: Create taskflow executors with {} threads, {} seperate threads, {} work threads, hyper threading is {}, taskflow version: {}",
		NumTotalThreads,
		NumSeperateThread,
		m_NumWorkThreads,
		m_UseHyperThreading ? "enabled" : "disabled",
		tf::version());
}

void TaskFlowService::OnShutdown()
{
	for (auto& Executor : m_Executors)
	{
		Executor->wait_for_all();
		Executor.reset();
	}
}
