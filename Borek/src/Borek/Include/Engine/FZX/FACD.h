// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/FZX/Body.h"
#include "Include/Engine/FZX/CGrid.h"
#include "Include/Engine/FZX/Collision.h"

namespace Borek {
namespace FZX {

class FACD {
public:
        FACD();
        // Adds a collider 
        uint32_t
        Add(uint32_t entity_id, Body& body, const glm::vec2& pos,
            const glm::vec2& size, float rotation = 0.0f);

        void
        Remove(uint32_t entity_id, const Body& body);

        void
        RemoveTilemapNode(uint32_t entity_id, uint32_t element_id);

        void
        Update(uint32_t entity_id, Body& collision_body,
               const glm::vec2& new_pos, const glm::vec2& new_size,
               float rotation = 0.0f);

        // Gets collisions for a body
        SmallList<uint32_t>
        GetCollisions(uint32_t entity_id, const Body& body);

        // Gets collisions for a point
        SmallList<uint32_t>
        GetCollisions(glm::vec2 position, uint32_t ignore_id = UINT32_MAX,
                      bool ignore_areas = true);

        std::pair<Collision, glm::vec2>
        MoveAndCollide(uint32_t entity_id, Body& body, float dx, float dy);

private:
        CGrid m_Grid;
        CGrid m_AreaGrid;
};

} // namespace FZX
}  // namespace Borek
