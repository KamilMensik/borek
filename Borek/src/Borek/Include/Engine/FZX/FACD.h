// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/FZX/Body.h"
#include "Include/Engine/FZX/CGrid.h"

namespace Borek {
namespace FZX {

class FACD {
public:
        FACD();
        // Adds a collider 
        void
        Add(uint32_t entity_id, const Body& collision_body);

        void
        Remove(uint32_t entity_id, const Body& collision_body);

        void
        Update(uint32_t entity_id, Body& collision_body,
               const glm::vec2& new_pos, const glm::vec2& new_size);

        // Gets collisions for a body
        SmallList<uint32_t>
        GetCollisions(uint32_t entity_id, const Body& body);

        // Gets collisions for a point
        SmallList<uint32_t>
        GetCollisions(glm::vec2 position, uint32_t ignore_id = UINT32_MAX);

        glm::vec2
        MoveAndCollide(uint32_t entity_id, Body& body,
                       float dx, float dy);

private:
        CGrid m_Grid;
};

} // namespace FZX
}  // namespace Borek
