#pragma once

#include "Core/StringUtils.h"
#include "Core/Module.h"

#define SPDLOG_LEVEL_NAMES                                              \
    {                                                                   \
        "Trace", "Debug", "Info", "Warning", "Error", "Critical", "Off" \
    }

#pragma warning(push)
#pragma warning(disable:4459)
#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/msvc_sink.h>
#pragma warning(pop)

using SpdLogMessage = spdlog::details::log_msg;
using SpdLogger = spdlog::logger;

enum class ELogLevel
{
	Trace = spdlog::level::trace,
	Debug = spdlog::level::debug,
	Info = spdlog::level::info,
	Warning = spdlog::level::warn,
	Error = spdlog::level::err,
	Critical = spdlog::level::critical,
	Num = spdlog::level::off
};

class SpdLogService : public IService<SpdLogService>
{
public:
	class LogRedirector
	{
	public:
		virtual void Log(const SpdLogMessage& Log) = 0;
	};

	SpdLogService();

	void RegisterRedirector(LogRedirector* Redirector);

	inline spdlog::logger& GetLogger(const char* Name) { return GetOrCreateLogger(Name, GetDefaultLevel()); }
	inline spdlog::logger& GetDefaultLogger() { return *m_DefaultLogger; }

	spdlog::logger& GetOrCreateLogger(const char* Name, ELogLevel Level);

	const std::unordered_set<LogRedirector*>& GetRedirectors() const { return m_Redirectors; }
private:
	class WinDebugSinkAsync : public spdlog::sinks::windebug_sink_mt
	{
	public:
		using spdlog::sinks::windebug_sink_mt::windebug_sink_mt;
	protected:
		void sink_it_(const spdlog::details::log_msg& Log) override;
	};

	static const char* GetDefaultPattern();
	static const ELogLevel GetDefaultLevel();

	spdlog::sink_ptr m_DefaultSink;
	std::shared_ptr<spdlog::logger> m_DefaultLogger;
	std::unordered_set<LogRedirector*> m_Redirectors;
	std::unordered_map<std::string_view, std::shared_ptr<spdlog::logger>> m_Loggers;

	std::mutex m_LoggerLock;
};

#define DECLARE_LOG_CATEGOTY(Category) \
	namespace spdlog { \
		extern spdlog::logger* LogCat_##Category; \
		spdlog::logger* GetLogger_##Category(); \
	}

#define DEFINE_LOG_CATEGORY(Category) \
	namespace spdlog { \
		spdlog::logger* LogCat_##Category = nullptr; \
		spdlog::logger* GetLogger_##Category() { \
			if (!LogCat_##Category) { \
				LogCat_##Category = &SpdLogService::Get().GetLogger(#Category); \
			} \
			return LogCat_##Category; \
		} \
	}

#define LOG_TRACE(Category, Format, ...) spdlog::GetLogger_##Category()->trace(Format, __VA_ARGS__)
#define LOG_DEBUG(Category, Format, ...) spdlog::GetLogger_##Category()->debug(Format, __VA_ARGS__)
#define LOG_INFO(Category, Format, ...) spdlog::GetLogger_##Category()->info(Format, __VA_ARGS__)
#define LOG_WARNING(Category, Format, ...) spdlog::GetLogger_##Category()->warn(Format, __VA_ARGS__)
#define LOG_ERROR(Category, Format, ...) spdlog::GetLogger_##Category()->error(Format, __VA_ARGS__)
#define LOG_CRITICAL(Category, Format, ...) spdlog::GetLogger_##Category()->critical(Format, __VA_ARGS__)

DECLARE_LOG_CATEGOTY(LogDefault);

