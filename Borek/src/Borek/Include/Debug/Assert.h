#include "Include/Debug/Log.h"

#ifndef RELEASE

#define BOREK_ENGINE_ASSERT(cond, ...)           \
        if (!(cond)) {                           \
                BOREK_ENGINE_ERROR(__VA_ARGS__); \
                std::exit(1);                    \
        }

#define BOREK_ASSERT(cond, ...)               \
        if (!(cond)) {                        \
                BOREK_LOG_ERROR(__VA_ARGS__); \
                std::exit(1);                 \
        }

#else

#define BOREK_ENGINE_ASSERT(cond, ...)
#define BOREK_ASSERT(cond, ...)

#endif
