#include "Services/SpdLogService.h"
#include <spdlog/details/registry.h>

void SpdLogService::WinDebugSinkAsync::sink_it_(const spdlog::details::log_msg& Log)
{
	spdlog::sinks::windebug_sink_mt::sink_it_(Log);

	for (auto Rediretor : SpdLogService::Get().GetRedirectors())
	{
		if (Rediretor)
		{
			Rediretor->Log(Log);
		}
	}
}

SpdLogService::SpdLogService()
{
	m_DefaultLogger = spdlog::create_async<WinDebugSinkAsync>("LogDefault", false);
	m_DefaultSink = m_DefaultLogger->sinks().front();

	m_DefaultLogger->set_level(static_cast<spdlog::level::level_enum>(GetDefaultLevel()));
	m_DefaultLogger->set_pattern(GetDefaultPattern());

	m_DefaultLogger->info("Use spdlog @{}", SPDLOG_VERSION);
}

const char* SpdLogService::GetDefaultPattern()
{
	return "[%n][%^%l%$]: %v";
}

const ELogLevel SpdLogService::GetDefaultLevel()
{
#if _DEBUG
	return ELogLevel::Trace;
#else
	return ELogLevel::Info;
#endif
}

spdlog::logger& SpdLogService::GetOrCreateLogger(const char* Name, ELogLevel Level)
{
	std::lock_guard Locker(m_LoggerLock);

	auto It = m_Loggers.find(Name);
	if (It == m_Loggers.end())
	{
		auto Logger = std::make_shared<spdlog::logger>(Name, m_DefaultSink);
		spdlog::initialize_logger(Logger);
		Logger->set_pattern(GetDefaultPattern());
		Logger->set_level(static_cast<spdlog::level::level_enum>(Level));
		It = m_Loggers.emplace(std::make_pair(std::string_view(Name), std::move(Logger))).first;
	}
	else
	{
		auto TargetLevel = static_cast<spdlog::level::level_enum>(Level);
		if (It->second->level() != TargetLevel)
		{
			It->second->set_level(TargetLevel);
		}
	}

	return *It->second;
}

void SpdLogService::RegisterRedirector(LogRedirector* Redirector)
{
	// TODO: Thread safe ???
	m_Redirectors.insert(Redirector);
}
