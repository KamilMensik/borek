// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include <glm/ext/vector_float2.hpp>

#include "Include/Platform/SIMD.h"

namespace Borek {
namespace FZX {

enum class BodyType : uint8_t {
        Static = 0,
        Dynamic = 1,
        Area = 2,
};

enum class ColliderType : uint8_t {
        Rectangle = 0,
        Tilemap = 1,
        Circle = 2,
};

struct RectangleCollider {
        uint16_t size_x, size_y;
        float rotation;
};

struct TilemapCollider {
        uint16_t row, col;
};

struct CircleCollider {
        uint16_t radius;
};

enum PhysicsFlags {
        PhysicsFlags_CollidedLeft = 1,
        PhysicsFlags_CollidedRight = 2,
        PhysicsFlags_CollidedTop = 4,
        PhysicsFlags_CollidedBottom = 8,
};

struct Body {
        uint32_t physics_flags;
        uint16_t collision_mask = UINT16_MAX;
        BodyType body_type;
        ColliderType collider_type = ColliderType::Rectangle;
        union {
                RectangleCollider rc;
                TilemapCollider tc;
                CircleCollider cc;
        };

        M128
        Update(const glm::vec2& pos, const glm::vec2& size, float rotation = 0);
};

}  // namespace FZX
}  // namespace Borek
