#include "Runtime/Engine/Services/TaskFlowService.h"
#include "Runtime/Engine/Services/SpdLogService.h"
#include "Runtime/Core/PlatformMisc.h"

TaskFlowService::TaskFlowService()
	: m_UseHyperThreading(true)
{
	auto ThreadsCount = PlatformMisc::GetHardwareConcurrencyThreadsCount(m_UseHyperThreading);
	m_Executor = std::make_unique<tf::Executor>(ThreadsCount);

	LOG_INFO("TaskFlow: Create taskflow executor with {} threads, hyper threading is {}, taskflow version: {}",
		ThreadsCount,
		m_UseHyperThreading ? "enabled" : "disabled",
		tf::version());
}
