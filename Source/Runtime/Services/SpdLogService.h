#pragma once

#include "Core/StringUtils.h"
#include "Core/Module.h"
#pragma warning(push)
#pragma warning(disable:4702)
#include <Submodules/spdlog/include/spdlog/spdlog.h>
#include <Submodules/spdlog/include/spdlog/async_logger.h>
#include <Submodules/spdlog/include/spdlog/sinks/windebug_sink.h>
#pragma warning(pop)

using SpdLogMessage = spdlog::details::log_msg;

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

	template<class... Arg>
	void Log(const char* Category, ELogLevel Level, const char* Format, Arg&&... Args)
	{
		if (!Category)
		{
			m_DefaultLogger->log(static_cast<spdlog::level::level_enum>(Level), Format, std::forward<Arg>(Args)...);
		}
		else
		{
			if (auto Logger = spdlog::get(Category))
			{
				Logger->log(static_cast<spdlog::level::level_enum>(Level), Format, std::forward<Arg>(Args)...);
			}
		}
	}

	void RegisterRedirector(LogRedirector* Redirector);

	void RegisterLogCategory(const char* Category, ELogLevel Level = ELogLevel::Debug);

	const std::unordered_set<LogRedirector*>& GetRedirectors() const { return m_Redirectors; }
private:
	class WinDebugSinkAsync : public spdlog::sinks::windebug_sink_mt
	{
	public:
		using BaseSink = spdlog::sinks::windebug_sink_mt;
		using BaseSink::BaseSink;

		WinDebugSinkAsync(SpdLogService& Service)
			: m_Service(Service)
		{
		}
	protected:
		void _sink_it(const spdlog::details::log_msg& Log) override;
	private:
		SpdLogService& m_Service;
	};

	std::shared_ptr<spdlog::logger> CreateLogger(const char* Name, ELogLevel Level);

	std::shared_ptr<spdlog::logger> m_DefaultLogger;
	std::unordered_set<LogRedirector*> m_Redirectors;
};

#define REGISTER_LOG_CATEGORY(Category) SpdLogService::Get().RegisterLogCategory(#Category)

#define LOG(Level, Format, ...) SpdLogService::Get().Log(nullptr, ELogLevel::Level, Format, __VA_ARGS__)
#define LOG_TRACE(Format, ...) SpdLogService::Get().Log(nullptr, ELogLevel::Trace, Format, __VA_ARGS__)
#define LOG_DEBUG(Format, ...) SpdLogService::Get().Log(nullptr, ELogLevel::Debug, Format, __VA_ARGS__)
#define LOG_INFO(Format, ...) SpdLogService::Get().Log(nullptr, ELogLevel::Info, Format, __VA_ARGS__)
#define LOG_WARNING(Format, ...) SpdLogService::Get().Log(nullptr, ELogLevel::Warning, Format, __VA_ARGS__)
#define LOG_ERROR(Format, ...) SpdLogService::Get().Log(nullptr, ELogLevel::Error, Format, __VA_ARGS__)
#define LOG_CRITICAL(Format, ...) SpdLogService::Get().Log(nullptr, ELogLevel::Critical, Format, __VA_ARGS__)

#define LOG_CAT(Category, Level, Format, ...) SpdLogService::Get().Log(#Category, ELogLevel::Level, Format, __VA_ARGS__)
#define LOG_CAT_TRACE(Category, Format, ...) SpdLogService::Get().Log(#Category, ELogLevel::Trace, Format, __VA_ARGS__)
#define LOG_CAT_DEBUG(Category, Format, ...) SpdLogService::Get().Log(#Category, ELogLevel::Debug, Format, __VA_ARGS__)
#define LOG_CAT_INFO(Category, Format, ...) SpdLogService::Get().Log(#Category, ELogLevel::Info, Format, __VA_ARGS__)
#define LOG_CAT_WARNING(Category, Format, ...) SpdLogService::Get().Log(#Category, ELogLevel::Warning, Format, __VA_ARGS__)
#define LOG_CAT_ERROR(Category, Format, ...) SpdLogService::Get().Log(#Category, ELogLevel::Error, Format, __VA_ARGS__)
#define LOG_CAT_CRITICAL(Category, Format, ...) SpdLogService::Get().Log(#Category, ELogLevel::Critical, Format, __VA_ARGS__)

