#include "Runtime/Engine/Modules/TaskFlowModule.h"
#include "Runtime/Core/PlatformMisc.h"
#include "Runtime/Engine/Engine.h"

TaskFlowModule::TaskFlowModule()
	: m_UseHyperThreading(true)
{
	auto ThreadsCount = PlatformMisc::GetHardwareConcurrencyThreadsCount(m_UseHyperThreading);
	m_Executor = std::make_unique<tf::Executor>(ThreadsCount);

	LOG_INFO("TaskFlow: Create taskflow executor with {} threads, hyper threading is {}, taskflow version: {}",
		ThreadsCount,
		m_UseHyperThreading ? "enabled" : "disabled",
		tf::version());
}
