#pragma once

#include "Core/StringUtils.h"
#include "Core/Module.h"

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

	inline std::shared_ptr<spdlog::logger> GetLogger(const char* Name)
	{
		return CreateLogger(Name, GetDefaultLevel());
	}

	inline std::shared_ptr<spdlog::logger> GetDefaultLogger()
	{
		return m_DefaultLogger;
	}

	std::shared_ptr<spdlog::logger> CreateLogger(const char* Name, ELogLevel Level);

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
};

#define SPD_LOG_CATEGORY(Category, Level, Format, ...) \
	SpdLogService::Get().GetLogger(#Category)->Level(Format, __VA_ARGS__)

#define SPD_LOG_DEFAULT(Level, Format, ...) \
	SpdLogService::Get().GetDefaultLogger()->Level(Format, __VA_ARGS__)

#define LOG_TRACE(Format, ...) SPD_LOG_DEFAULT(trace, Format, __VA_ARGS__)
#define LOG_DEBUG(Format, ...) SPD_LOG_DEFAULT(debug, Format, __VA_ARGS__)
#define LOG_INFO(Format, ...) SPD_LOG_DEFAULT(info, Format, __VA_ARGS__)
#define LOG_WARNING(Format, ...) SPD_LOG_DEFAULT(warn, Format, __VA_ARGS__)
#define LOG_ERROR(Format, ...) SPD_LOG_DEFAULT(error, Format, __VA_ARGS__)
#define LOG_CRITICAL(Format, ...) SPD_LOG_DEFAULT(critical, Format, __VA_ARGS__)

#define LOG_TRACE_CAT(Category, Format, ...) SPD_LOG_CATEGORY(Category, trace, Format, __VA_ARGS__)
#define LOG_DEBUG_CAT(Category, Format, ...) SPD_LOG_CATEGORY(Category, debug, Format, __VA_ARGS__)
#define LOG_INFO_CAT(Category, Format, ...) SPD_LOG_CATEGORY(Category, info, Format, __VA_ARGS__)
#define LOG_WARNING_CAT(Category, Format, ...) SPD_LOG_CATEGORY(Category, warn, Format, __VA_ARGS__)
#define LOG_ERROR_CAT(Category, Format, ...) SPD_LOG_CATEGORY(Category, error, Format, __VA_ARGS__)
#define LOG_CRITICAL_CAT(Category, Format, ...) SPD_LOG_CATEGORY(Category, critical, Format, __VA_ARGS__)

