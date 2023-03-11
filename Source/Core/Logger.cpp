#include "Core/Logger.h"
#include <ThirdParty/spdlog/include/spdlog/sinks/windebug_sink.h>

NAMESPACE_START(Gear)

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

NAMESPACE_END(Gear)