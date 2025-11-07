#include "Services/SpdLogService.h"

void SpdLogService::WinDebugSinkAsync::sink_it_(const spdlog::details::log_msg& Log)
{
	spdlog::sinks::windebug_sink_mt::sink_it_(Log);

	for (auto Sink : m_Service.GetRedirectors())
	{
		if (Sink)
		{
			Sink->Log(Log);
		}
	}
}

SpdLogService::SpdLogService()
{
	m_DefaultLogger = CreateLogger("LogDefault", 
#if _DEBUG
		ELogLevel::Trace
#else
		ELogLevel::Info
#endif
	);
	m_DefaultLogger->info("Use spdlog @{}", SPDLOG_VERSION);
}

std::shared_ptr<spdlog::logger> SpdLogService::CreateLogger(const char* Name, ELogLevel Level)
{
	auto Logger = spdlog::create_async<WinDebugSinkAsync>(Name, *this);
	Logger->set_level(static_cast<spdlog::level::level_enum>(Level));
	Logger->set_pattern("[%n]: [%^%l%$]: %v");
	return Logger;
}

void SpdLogService::RegisterRedirector(LogRedirector* Redirector)
{
	// TODO: Thread safe ???
	m_Redirectors.insert(Redirector);
}

void SpdLogService::RegisterLogCategory(const char* Category, ELogLevel Level)
{
	std::string LoggerName = StringUtils::Lowercase(std::string(Category));
	if (!spdlog::get(LoggerName))
	{
		CreateLogger(Category, Level);
	}
}
