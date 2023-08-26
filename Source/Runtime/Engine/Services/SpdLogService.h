#pragma once

#include "Runtime/Core/StringUtils.h"
#include "Runtime/Core/Module.h"
#pragma warning(push)
#pragma warning(disable:4702)
#include <Submodules/spdlog/include/spdlog/spdlog.h>
#include <Submodules/spdlog/include/spdlog/async_logger.h>
#include <Submodules/spdlog/include/spdlog/sinks/windebug_sink.h>
#pragma warning(pop)

class SpdLogService : public IService<SpdLogService>
{
public:
	SpdLogService()
	{
		m_AsyncLogger = spdlog::create_async("SpdLogger", std::make_shared<spdlog::sinks::windebug_sink_mt>(), 1024u);
		m_AsyncLogger->set_pattern("[%^%l%$] %v");

#if _DEBUG
		m_AsyncLogger->set_level(spdlog::level::trace);
#else
		m_AsyncLogger->set_level(spdlog::level::info);
#endif
	}

	template<class... Args>
	void Info(const char8_t* Format, Args&&... ArgList) { m_AsyncLogger->info(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	void Warning(const char8_t* Format, Args&&... ArgList) { m_AsyncLogger->warn(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	void Error(const char8_t* Format, Args&&... ArgList) { m_AsyncLogger->error(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	void Debug(const char8_t* Format, Args&&... ArgList) { m_AsyncLogger->debug(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	void Trace(const char8_t* Format, Args&&... ArgList) { m_AsyncLogger->trace(Format, std::forward<Args>(ArgList)...); }

	template<class... Args>
	void Critical(const char8_t* Format, Args&&... ArgList) { m_AsyncLogger->critical(Format, std::forward<Args>(ArgList)...); }
private:
	std::shared_ptr<spdlog::logger> m_AsyncLogger;
};

#define LOG_INFO(Format, ...)       SpdLogService::Get().Info(Format, __VA_ARGS__)
#define LOG_WARNING(Format, ...)    SpdLogService::Get().Warning(Format, __VA_ARGS__)
#define LOG_ERROR(Format, ...)      SpdLogService::Get().Error(Format, __VA_ARGS__)
#define LOG_DEBUG(Format, ...)      SpdLogService::Get().Debug(Format, __VA_ARGS__)
#define LOG_CRITICAL(Format, ...)   SpdLogService::Get().Critical(Format, __VA_ARGS__)
#define LOG_TRACE(Format, ...)      SpdLogService::Get().Trace(Format, __VA_ARGS__)

