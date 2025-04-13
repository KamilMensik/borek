#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "Include/Log.h"

namespace Borek {

void Log::Init() {
        spdlog::set_pattern("%^[%T %n]: %v%$");
        m_Client = spdlog::stdout_color_mt("Client");
        m_Client->set_level(spdlog::level::trace);

        m_Engine = spdlog::stdout_color_mt("BorekEngine");
        m_Engine->set_level(spdlog::level::trace);
}

std::shared_ptr<spdlog::logger>& Log::Engine() {
        return m_Engine;
}

std::shared_ptr<spdlog::logger>& Log::Client() {
        return m_Client;
}

std::shared_ptr<spdlog::logger> Log::m_Client;
std::shared_ptr<spdlog::logger> Log::m_Engine;

}  // namespace Borek
