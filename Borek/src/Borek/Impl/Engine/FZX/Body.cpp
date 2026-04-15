// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cmath>

#include <glm/trigonometric.hpp>

#include "Include/Engine/FZX/Body.h"
#include "Include/Platform/SIMD.h"

namespace Borek {
namespace FZX {

inline static M128
get_rect_aabb(const glm::vec2& pos, const glm::vec2& size, const Body& body)
{
        if (body.rc.rotation == 0) {
                const M128 pos_v = simd_create4f(pos.x, pos.y, pos.x, pos.y);
                const M128 size_v = simd_create4f(0, 0, size.x * body.rc.size_x,
                                                  size.y * body.rc.size_y);
                return simd_add4f(pos_v, size_v);
        }

        const M128 s = simd_scalar4f(std::sin(glm::radians(body.rc.rotation)));
        const M128 c = simd_scalar4f(std::cos(glm::radians(body.rc.rotation)));

        M128 points_x = simd_create4f(0, size.x * body.rc.size_x,
                                      size.x * body.rc.size_x, 0);

        M128 points_y = simd_create4f(0, 0, size.y * body.rc.size_y,
                                      size.y * body.rc.size_y);

        const M128 r_points_x = simd_add4f(
                simd_sub4f(simd_mul4f(points_x, c), simd_mul4f(points_y, s)),
                simd_scalar4f(pos.x));

        const M128 r_points_y = simd_add4f(
                simd_add4f(simd_mul4f(points_x, s), simd_mul4f(points_y, c)),
                simd_scalar4f(pos.y));

        return simd_create4f(simd_fmin4f(r_points_x), simd_fmin4f(r_points_y),
                             simd_fmax4f(r_points_x), simd_fmax4f(r_points_y));
}

M128
Body::Update(const glm::vec2& pos, const glm::vec2& size, float rotation)
{

        switch (collider_type) {
        case ColliderType::Rectangle: {
                rc.rotation = rotation;
                return get_rect_aabb(pos, size, *this);
        }
        case ColliderType::Tilemap: {
                const M128 pos_v = simd_create4f(pos.x, pos.y, pos.x, pos.y);
                const M128 size_v = simd_create4f(0, 0, size.x, size.y);
                return simd_add4f(pos_v, size_v);
        }
        case ColliderType::Circle: {
                const M128 pos_v = simd_create4f(pos.x, pos.y, pos.x, pos.y);
                const M128 size_v = simd_create4f(0, 0, size.x * cc.radius,
                                                  size.y * cc.radius);
                return simd_add4f(pos_v, size_v);
        }
        }
}

} // namespace FZX
}  // namespace Borek
