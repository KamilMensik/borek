#pragma once

#ifndef RELEASE

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "Include/Core.h"

namespace Borek {

class Log {
public:
        static void Init();
        static inline spdlog::logger& Engine() { return *s_Engine; }
        static inline spdlog::logger& Client() { return *s_Client; }

private:
        static Ref<spdlog::logger> s_Engine;
        static Ref<spdlog::logger> s_Client;
};

// m_Client Logging Macros
#define BOREK_LOG_WARN(...) Borek::Log::Client().warn(__VA_ARGS__)
#define BOREK_LOG_ERROR(...) Borek::Log::Client().error(__VA_ARGS__)
#define BOREK_LOG_TRACE(...) Borek::Log::Client().trace(__VA_ARGS__)
#define BOREK_LOG_INFO(...) Borek::Log::Client().info(__VA_ARGS__)
#define BOREK_LOG_FATAL(...) Borek::Log::Client().critical(__VA_ARGS__)


// m_Engine Logging Macros
#define BOREK_ENGINE_WARN(...) Borek::Log::Engine().warn(__VA_ARGS__)
#define BOREK_ENGINE_ERROR(...) Borek::Log::Engine().error(__VA_ARGS__)
#define BOREK_ENGINE_TRACE(...) Borek::Log::Engine().trace(__VA_ARGS__)
#define BOREK_ENGINE_INFO(...) Borek::Log::Engine().info(__VA_ARGS__)
#define BOREK_ENGINE_FATAL(...) Borek::Log::Engine().critical(__VA_ARGS__)

}  // namespace Borek

#endif
