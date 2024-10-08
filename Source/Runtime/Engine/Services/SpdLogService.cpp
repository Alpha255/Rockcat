#include "Engine/Services/SpdLogService.h"

void SpdLogService::Logger::RegisterRedirector(spdlog::sinks::sink* Sink)
{
	m_Redirectors.insert(Sink);
}

void SpdLogService::Logger::_sink_it(const spdlog::details::log_msg& Message)
{
	spdlog::sinks::windebug_sink_mt::_sink_it(Message);

	for (auto Sink : m_Redirectors)
	{
		if (Sink)
		{
			Sink->log(Message);
		}
	}
}

SpdLogService::SpdLogService()
{
	m_AsyncLogger = spdlog::create_async("SpdLogger", std::make_shared<Logger>(), 1024u);
	m_AsyncLogger->set_pattern("[%^%l%$] %v");

#if _DEBUG
	m_AsyncLogger->set_level(spdlog::level::trace);
#else
	m_AsyncLogger->set_level(spdlog::level::info);
#endif
}

void SpdLogService::RegisterRedirector(spdlog::sinks::sink* Sink)
{
	Cast<Logger>(m_AsyncLogger->sinks()[0].get())->RegisterRedirector(Sink);
}
