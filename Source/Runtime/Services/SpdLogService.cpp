#include "Services/SpdLogService.h"

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

	m_DefaultLogger->set_level(static_cast<spdlog::level::level_enum>(GetDefaultLogLevel()));
	m_DefaultLogger->set_pattern(GetDefaultPattern());

	m_DefaultLogger->info("Use spdlog @{}", SPDLOG_VERSION);
}

const char* SpdLogService::GetDefaultPattern()
{
	return "[%n][%^%l%$]: %v";
}

const ELogLevel SpdLogService::GetDefaultLogLevel()
{
#if _DEBUG
	return ELogLevel::Trace;
#else
	return ELogLevel::Info;
#endif
}

std::shared_ptr<spdlog::logger> SpdLogService::CreateLogger(const char* Name, ELogLevel Level)
{
	auto LowercaseName = StringUtils::Lowercase(Name);
	auto Logger = spdlog::get(LowercaseName);
	if (!Logger)
	{
		Logger = std::make_shared<spdlog::logger>(LowercaseName, m_DefaultSink);
		spdlog::initialize_logger(Logger);
		Logger->set_pattern(GetDefaultPattern());
		Logger->set_level(static_cast<spdlog::level::level_enum>(Level));
	}
	else
	{
		auto TargetLevel = static_cast<spdlog::level::level_enum>(Level);
		if (Logger->level() != TargetLevel)
		{
			Logger->set_level(TargetLevel);
		}
	}

	return Logger;
}

void SpdLogService::RegisterRedirector(LogRedirector* Redirector)
{
	// TODO: Thread safe ???
	m_Redirectors.insert(Redirector);
}
