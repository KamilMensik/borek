#pragma once

#include <format>

#ifndef RELEASE

namespace Borek {

class Log {
public:
        template<typename ... Args> static void
        Warn(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtGame, s_ColorWarn, "Game",
                                 time, msg));
        }

        template<typename ... Args> static void
        Error(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtGame, s_ColorError, "Game",
                                 time, msg));
        }

        template<typename ... Args> static void
        Trace(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtGame, s_ColorTrace, "Game",
                                 time, msg));
        }

        template<typename ... Args> static void
        Info(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtGame, s_ColorInfo, "Game",
                                 time, msg));
        }

        template<typename ... Args> static void
        Critical(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtGame, s_ColorCritical, "Game",
                                 time, msg));
        }

        template<typename ... Args> static void
        EngineWarn(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtEngine, s_ColorWarn,
                                 time, msg));
        }

        template<typename ... Args> static void
        EngineError(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtEngine, s_ColorError,
                                             time, msg));
        }

        template<typename ... Args> static void
        EngineTrace(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtEngine, s_ColorTrace,
                                 time, msg));
        }

        template<typename ... Args> static void
        EngineInfo(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtEngine, s_ColorInfo,
                                 time, msg));
        }

        template<typename ... Args> static void
        EngineCritical(const std::format_string<Args...>& fmt, Args&&... args)
        {
                const std::string time = GetTime();
                const std::string msg = std::vformat(fmt.get(), std::make_format_args(args...));
                Send(std::format(s_FmtEngine, s_ColorCritical,
                                 time, msg));
        }

private:
        static std::string GetTime();

        static void Send(const std::string& str);

        constexpr static const char* s_FmtEngine = "{}[{} BorekEngine]: {}";
        constexpr static const char* s_FmtGame = "{}[{} {}]: {}";

        constexpr static const char* s_ColorWarn = "\033[33m\033[49m";
        constexpr static const char* s_ColorError = "\033[31m\033[49m";
        constexpr static const char* s_ColorTrace = "\033[32m\033[49m";
        constexpr static const char* s_ColorInfo = "\033[34m\033[49m";
        constexpr static const char* s_ColorCritical = "\033[37m\033[41m";
};

// m_Client Logging Macros
#define BOREK_LOG_WARN(...) Borek::Log::Warn(__VA_ARGS__)
#define BOREK_LOG_ERROR(...) Borek::Log::Error(__VA_ARGS__)
#define BOREK_LOG_TRACE(...) Borek::Log::Trace(__VA_ARGS__)
#define BOREK_LOG_INFO(...) Borek::Log::Info(__VA_ARGS__)
#define BOREK_LOG_FATAL(...) Borek::Log::Critical(__VA_ARGS__)


// m_Engine Logging Macros
#define BOREK_ENGINE_WARN(...) Borek::Log::EngineWarn(__VA_ARGS__)
#define BOREK_ENGINE_ERROR(...) Borek::Log::EngineError(__VA_ARGS__)
#define BOREK_ENGINE_TRACE(...) Borek::Log::EngineTrace(__VA_ARGS__)
#define BOREK_ENGINE_INFO(...) Borek::Log::EngineInfo(__VA_ARGS__)
#define BOREK_ENGINE_FATAL(...) Borek::Log::EngineCritical(__VA_ARGS__)

}  // namespace Borek

#endif
