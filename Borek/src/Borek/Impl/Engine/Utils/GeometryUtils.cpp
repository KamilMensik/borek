// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <glm/ext/vector_float2.hpp>
#include <glm/trigonometric.hpp>

#include "Include/Engine/Utils/GeometryUtils.h"

namespace Borek {
namespace Utils {
namespace Geometry {

glm::vec2
rotate_point(const glm::vec2& origin, const glm::vec2& point, float angle)
{
        float s = std::sin(glm::radians(angle));
        float c = std::cos(glm::radians(angle));

        const glm::vec2 p = point - origin;
        const float xoff = p.x * c - p.y * s;
        const float yoff = p.x * s + p.y * c;

        return point + glm::vec2(xoff, yoff);
}

}  // namespace Geometry
}  // namespace Utils
}  // namespace Borek
