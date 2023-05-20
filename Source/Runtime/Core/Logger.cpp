#include "Runtime/Core/Logger.h"
#include <Submodules/spdlog/include/spdlog/sinks/windebug_sink.h>

Logger::Logger()
{
	m_AsyncLogger = spdlog::create_async("SpdLogger", std::make_shared<spdlog::sinks::windebug_sink_mt>(), 1024u);
	m_AsyncLogger->set_pattern("[%^%l%$] %v");
#if _DEBUG
	m_AsyncLogger->set_level(spdlog::level::trace);
#else
	m_AsyncLogger->set_level(spdlog::level::info);
#endif
}

