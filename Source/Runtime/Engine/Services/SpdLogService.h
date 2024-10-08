#pragma once

#include "Core/StringUtils.h"
#include "Core/Module.h"
#pragma warning(push)
#pragma warning(disable:4702)
#include <Submodules/spdlog/include/spdlog/spdlog.h>
#include <Submodules/spdlog/include/spdlog/async_logger.h>
#include <Submodules/spdlog/include/spdlog/sinks/windebug_sink.h>
#pragma warning(pop)

class SpdLogService : public IService<SpdLogService>
{
public:
	SpdLogService();

	template<class... Args>
	inline void Info(const char* Format, Args&&... ArgList) { m_AsyncLogger->info(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	inline void Warning(const char* Format, Args&&... ArgList) { m_AsyncLogger->warn(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	inline void Error(const char* Format, Args&&... ArgList) { m_AsyncLogger->error(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	inline void Debug(const char* Format, Args&&... ArgList) { m_AsyncLogger->debug(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	inline void Trace(const char* Format, Args&&... ArgList) { m_AsyncLogger->trace(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	inline void Critical(const char* Format, Args&&... ArgList) { m_AsyncLogger->critical(Format, std::forward<Args>(ArgList)...); }

	void RegisterRedirector(spdlog::sinks::sink* Sink);
private:
	class Logger : public spdlog::sinks::windebug_sink_mt
	{
	public:
		void RegisterRedirector(spdlog::sinks::sink* Sink);
	protected:
		void _sink_it(const spdlog::details::log_msg& Message) override;
	private:
		std::unordered_set<spdlog::sinks::sink*> m_Redirectors;
	};

	std::shared_ptr<spdlog::logger> m_AsyncLogger;
};

#define LOG_INFO(Format, ...)       SpdLogService::Get().Info(Format, __VA_ARGS__)
#define LOG_WARNING(Format, ...)    SpdLogService::Get().Warning(Format, __VA_ARGS__)
#define LOG_ERROR(Format, ...)      SpdLogService::Get().Error(Format, __VA_ARGS__)
#define LOG_DEBUG(Format, ...)      SpdLogService::Get().Debug(Format, __VA_ARGS__)
#define LOG_CRITICAL(Format, ...)   SpdLogService::Get().Critical(Format, __VA_ARGS__)
#define LOG_TRACE(Format, ...)      SpdLogService::Get().Trace(Format, __VA_ARGS__)

