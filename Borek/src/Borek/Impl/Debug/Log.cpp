#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "Include/Debug/Log.h"

namespace Borek {

void Log::Init() {
        spdlog::set_pattern("%^[%T %n]: %v%$");
        s_Client = spdlog::stdout_color_mt("Client");
        s_Client->set_level(spdlog::level::trace);

        s_Engine = spdlog::stdout_color_mt("BorekEngine");
        s_Engine->set_level(spdlog::level::trace);
}

Ref<spdlog::logger> Log::s_Client;
Ref<spdlog::logger> Log::s_Engine;

}  // namespace Borek
