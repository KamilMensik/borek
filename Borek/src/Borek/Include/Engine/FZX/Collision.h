// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <string>
#include <format>

#include "glm/ext/vector_float2.hpp"

namespace Borek {
namespace FZX {

struct Collision {
        uint32_t collider = UINT32_MAX;
        glm::vec2 normal = glm::vec2(0.0f, 0.0f);
        union {
                struct { int16_t row; int16_t col; } tilemap_data;
        };

        inline operator bool() const { return collider != UINT32_MAX; }
        operator std::string() const
        {
                return std::format("<Collision: [{}, [{}, {}]]",
                                   collider, normal.x, normal.y);
        }
};

}  // namespace FZX
}  // namespace Borek
