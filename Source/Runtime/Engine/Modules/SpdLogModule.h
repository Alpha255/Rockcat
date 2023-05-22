#pragma once

#include "Runtime/Core/StringUtils.h"
#include "Runtime/Core/Module.h"
#include <Submodules/spdlog/include/spdlog/spdlog.h>
#include <Submodules/spdlog/include/spdlog/async_logger.h>

class SpdLogModule : public IModule
{
public:
	SpdLogModule();

	template<class... Args>
	void Info(const char8_t* Format, Args&&... ArgList)
	{
		m_AsyncLogger->info(Format, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Warning(const char8_t* Format, Args&&... ArgList)
	{
		m_AsyncLogger->warn(Format, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Error(const char8_t* Format, Args&&... ArgList)
	{
		m_AsyncLogger->error(Format, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Debug(const char8_t* Format, Args&&... ArgList)
	{
		m_AsyncLogger->debug(Format, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Trace(const char8_t* Format, Args&&... ArgList)
	{
		m_AsyncLogger->trace(Format, std::forward<Args>(ArgList)...);
	}

	template<class... Args>
	void Critical(const char8_t* Format, Args&&... ArgList)
	{
		m_AsyncLogger->critical(Format, std::forward<Args>(ArgList)...);
	}
private:
	std::shared_ptr<spdlog::logger> m_AsyncLogger;
};

#define LOG_INFO(Format, ...)       Engine::Get().GetSpdLogModule().Info(Format, __VA_ARGS__)
#define LOG_WARNING(Format, ...)    Engine::Get().GetSpdLogModule().Warning(Format, __VA_ARGS__)
#define LOG_ERROR(Format, ...)      Engine::Get().GetSpdLogModule().Error(Format, __VA_ARGS__)
#define LOG_DEBUG(Format, ...)      Engine::Get().GetSpdLogModule().Debug(Format, __VA_ARGS__)
#define LOG_CRITICAL(Format, ...)   Engine::Get().GetSpdLogModule().Critical(Format, __VA_ARGS__)
#define LOG_TRACE(Format, ...)      Engine::Get().GetSpdLogModule().Trace(Format, __VA_ARGS__)

