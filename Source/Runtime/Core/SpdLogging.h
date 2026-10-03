#pragma once

#define SPDLOG_LEVEL_NAMES                                              \
	{                                                                   \
		"Trace", "Debug", "Info", "Warning", "Error", "Critical", "Off" \
	}

#include "Core/Singleton.h"
#include "Core/Name.h"
#include <spdlog/spdlog.h>

class SpdLogging : public Singleton<SpdLogging>
{
public:
	SpdLogging();

	spdlog::logger& GetLogger(Name LoggerName);
	spdlog::logger& GetOrCreateLogger(Name LoggerName, spdlog::level::level_enum Level, string Pattern);

private:
	static const char* GetDebugPattern()
	{
		return "[%n][%^%l%$]: %v";
	}
	
	static const char* GetFilePattern()
	{
		return "[%n][%^%l%$][%s:%#]: %v";
	}

	static spdlog::level::level_enum GetDefaultLevel()
	{
#if defined(_DEBUG)
		return spdlog::level::trace;
#else
		return spdlog::level::info;
#endif
	}

	std::mutex m_Lock;
	spdlog::sink_ptr m_DebugSink;
	spdlog::sink_ptr m_FileSink;
	std::unordered_map<Name, std::shared_ptr<spdlog::logger>> m_Loggers;
};

#define DECLARE_LOGGER_CATEGORY(Category) \
	extern spdlog::logger& SpdLogger_##Category;

#define DEFINE_LOGGER_CATEGORY(Category) \
	spdlog::logger& SpdLogger_##Category = SpdLogging::Get().GetLogger(#Category);

#define SPD_LOG(Category, Level, ...) \
	SpdLogger_##Category.log(spdlog::source_loc{ __FILE__, __LINE__, SPDLOG_FUNCTION }, Level, __VA_ARGS__)

#define LOG_TRACE(Category, ...)    SPD_LOG(Category, spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(Category, ...)    SPD_LOG(Category, spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(Category, ...)     SPD_LOG(Category, spdlog::level::info, __VA_ARGS__)
#define LOG_WARNING(Category, ...)  SPD_LOG(Category, spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(Category, ...)    SPD_LOG(Category, spdlog::level::err, __VA_ARGS__)
#define LOG_CRITICAL(Category, ...) SPD_LOG(Category, spdlog::level::critical, __VA_ARGS__)

DECLARE_LOGGER_CATEGORY(LogDefault);

#undef SPDLOG_LEVEL_NAMES
