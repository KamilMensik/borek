// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "glm/ext/vector_float2.hpp"
#include <cstdint>

#include "Include/Engine/FZX/Collider.h"

/*
*       Collision boxes for Borek.
*/

namespace Borek {
namespace FZX {

enum class BodyType : uint8_t {
        Static = 0,
        Dynamic = 1,
};

enum class ColliderType : uint8_t {
        Rectangle = 0,
        Circle = 1,
};

enum PhysicsFlags {
        PhysicsFlags_CollidedLeft = 1,
        PhysicsFlags_CollidedRight = 2,
        PhysicsFlags_CollidedTop = 4,
        PhysicsFlags_CollidedBottom = 8,
};

struct Body {
        Collider collider;
        uint32_t physics_flags;
        uint16_t collision_mask = UINT16_MAX;
        BodyType body_type = BodyType::Static;
        ColliderType collider_type = ColliderType::Rectangle;

        bool
        CollidesWith(const Body& other) const;

        void
        Update(const glm::vec2& position, const glm::vec2& size);
};

using BodyComponent = Body;

}  // namespace FZX
}  // namespace Borek
