// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <glm/ext/vector_float2.hpp>

namespace Borek {
namespace Utils {
namespace Geometry {

glm::vec2
rotate_point(const glm::vec2& origin, const glm::vec2& point, float angle);


}  // namespace Geometry
}  // namespace Utils
}  // namespace Borek
