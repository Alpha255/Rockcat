#include "Core/SpdLogging.h"
#include "Misc/Paths.h"
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

DEFINE_LOGGER_CATEGORY(LogDefault);

SpdLogging::SpdLogging()
{
	m_DebugSink = std::make_shared<spdlog::sinks::windebug_sink_mt>(true);
	m_DebugSink->set_pattern(GetDebugPattern());
	m_DebugSink->set_level(GetDefaultLevel());
	
	const std::filesystem::path LogPath = Paths::LogPath();
	const string LogFileName = string::format("{}\\{:%Y.%m.%d.%H.%M.%S}.log", LogPath.string(), std::chrono::system_clock::now());
	m_FileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(LogFileName, true);
	m_FileSink->set_pattern(GetFilePattern());
	m_FileSink->set_level(spdlog::level::trace);
}

spdlog::logger& SpdLogging::GetLogger(Name LoggerName)
{
	return GetOrCreateLogger(LoggerName, GetDefaultLevel(), GetDebugPattern());
}

spdlog::logger& SpdLogging::GetOrCreateLogger(Name LoggerName, spdlog::level::level_enum Level, string Pattern)
{
	std::lock_guard Locker(m_Lock);

	if (auto It = m_Loggers.find(LoggerName); It != m_Loggers.end())
	{
		return *It->second;
	}

	const std::vector<spdlog::sink_ptr> Sinks{ m_DebugSink, m_FileSink };

	auto Logger = std::make_shared<spdlog::logger>(std::string(LoggerName.Get()), Sinks.begin(), Sinks.end());
	Logger->set_level(Level);
	Logger->set_pattern(Pattern);
	/// Without an explicit flush policy the sink only spills every few kilobytes, so anything logged
	/// right before a crash (or a killed process) used to be lost from the log file.
	Logger->flush_on(spdlog::level::trace);

	return *m_Loggers.emplace(LoggerName, std::move(Logger)).first->second;
}