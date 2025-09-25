#include <chrono>
#include <ctime>

#include "Include/Debug/Log.h"
#include "Include/Base/Application.h"

namespace Borek {

void Log::Send(const std::string& str)
{
        Application::Log(str);       
}

std::string
Log::GetTime()
{
        const auto timenow = std::chrono::system_clock::now();
        const time_t t = std::chrono::system_clock::to_time_t(timenow);
        const std::tm* time_info = std::localtime(&t);

        std::ostringstream res;
        res << std::put_time(time_info, "%H:%M:%S");
        return res.str();
}

}  // namespace Borek
