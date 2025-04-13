#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include "./Core.h"

namespace Borek {

class Log {
public:
        static void Init();
        static std::shared_ptr<spdlog::logger>& Engine();
        static std::shared_ptr<spdlog::logger>& Client();

private:
        static std::shared_ptr<spdlog::logger> m_Engine;
        static std::shared_ptr<spdlog::logger> m_Client;
};

// m_Client Logging Macros
#define BOREK_LOG_WARN(...) Borek::Log::Client()->warn(__VA_ARGS__)
#define BOREK_LOG_ERROR(...) Borek::Log::Client()->error(__VA_ARGS__)
#define BOREK_LOG_TRACE(...) Borek::Log::Client()->trace(__VA_ARGS__)
#define BOREK_LOG_INFO(...) Borek::Log::Client()->info(__VA_ARGS__)
#define BOREK_LOG_FATAL(...) Borek::Log::Client()->critical(__VA_ARGS__)


// m_Engine Logging Macros
#define BOREK_ENGINE_WARN(...) Borek::Log::Engine()->warn(__VA_ARGS__)
#define BOREK_ENGINE_ERROR(...) Borek::Log::Engine()->error(__VA_ARGS__)
#define BOREK_ENGINE_TRACE(...) Borek::Log::Engine()->trace(__VA_ARGS__)
#define BOREK_ENGINE_INFO(...) Borek::Log::Engine()->info(__VA_ARGS__)
#define BOREK_ENGINE_FATAL(...) Borek::Log::Engine()->critical(__VA_ARGS__)

}  // namespace Borek
