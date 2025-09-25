// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <glm/ext/vector_float4.hpp>

namespace Borek {
namespace Colors {

static glm::vec4 FROM_RGB(uint8_t r, uint8_t g, uint8_t b)
{
        return glm::vec4((float)r / 256, float(g) / 256, float(b) / 256, 1.0f);
}

static constexpr glm::vec4 FROM_RGBA(uint8_t r, uint8_t g, uint8_t b, float a)
{
        return glm::vec4((float)r / 256, float(g) / 256, float(b) / 256, a);
}

static const glm::vec4 RED = FROM_RGB(242, 38, 19);
static const glm::vec4 GREEN = FROM_RGB(46, 204, 113);
static const glm::vec4 BLUE = FROM_RGB(43, 44, 170);
static const glm::vec4 YELLOW = FROM_RGB(255, 191, 0);
static const glm::vec4 CYAN = FROM_RGB(0, 238, 222);
static const glm::vec4 WHITE = glm::vec4(1.0f);

}  // namespace Colors
}  // namespace Borek
