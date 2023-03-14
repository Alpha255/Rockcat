#pragma once

#include "Runtime/Core/String.h"
#include "Runtime/Core/Singleton.h"
#include <Submodules/spdlog/include/spdlog/spdlog.h>
#include <Submodules/spdlog/include/spdlog/async_logger.h>

NAMESPACE_START(Gear)

class Logger : public Singleton<Logger>
{
public:
	template<class... Args>
	void Info(const char8_t* Fmt, Args&&... ArgList)
	{
		m_AsyncLogger->info(Fmt, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Warning(const char8_t* Fmt, Args&&... ArgList)
	{
		m_AsyncLogger->warn(Fmt, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Error(const char8_t* Fmt, Args&&... ArgList)
	{
		m_AsyncLogger->error(Fmt, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Debug(const char8_t* Fmt, Args&&... ArgList)
	{
		m_AsyncLogger->debug(Fmt, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Trace(const char8_t* Fmt, Args&&... ArgList)
	{
		m_AsyncLogger->trace(Fmt, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Critical(const char8_t* Fmt, Args&&... ArgList)
	{
		m_AsyncLogger->critical(Fmt, std::forward<Args>(ArgList)...);
	}
protected:
	ALLOW_ACCESS(Logger)

	Logger();
private:
	std::shared_ptr<spdlog::logger> m_AsyncLogger;
};

#define LOG_INFO(Format, ...)       Gear::Logger::Get().Info(Format, __VA_ARGS__)
#define LOG_WARNING(Format, ...)    Gear::Logger::Get().Warning(Format, __VA_ARGS__)
#define LOG_ERROR(Format, ...)      Gear::Logger::Get().Error(Format, __VA_ARGS__)
#define LOG_DEBUG(Format, ...)      Gear::Logger::Get().Debug(Format, __VA_ARGS__)
#define LOG_CRITICAL(Format, ...)   Gear::Logger::Get().Critical(Format, __VA_ARGS__)
#define TRACE(Format, ...)          Gear::Logger::Get().Trace(Format, __VA_ARGS__)

NAMESPACE_END(Gear)
