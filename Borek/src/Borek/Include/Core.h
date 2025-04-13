#pragma once

#include <cstdint>

#include "./Log.h"

#ifndef RELEASE
#define BOREK_ENGINE_ASSERT(cond, ...)           \
        if (!(cond)) {                             \
                BOREK_ENGINE_ERROR(__VA_ARGS__); \
                std::exit(1);                    \
        }
#define BOREK_ASSERT(cond, ...)               \
        if (!(cond)) {                          \
                BOREK_LOG_ERROR(__VA_ARGS__); \
                std::exit(1);                 \
        }
#else
#define BOREK_ENGINE_ASSERT(cond, ...)
#define BOREK_ASSERT(cond, ...)

#endif

#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

#define SCAST static_cast
#define DCAST dynamic_cast

namespace Borek {

// Enum with its members values being its sizes.
enum class Datatype : uint8_t {
        Float,
        Float2,
        Float3,
        Float4,
};

uint32_t DatatypeSize(Datatype d);

}  // namespace Borek

