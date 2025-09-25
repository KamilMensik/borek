// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/FZX/FACD.h"
#include "Include/Platform/SIMD.h"

namespace Borek {
namespace FZX {

FACD::FACD() : m_Grid(-1536, -1536, 1536, 1536, 96, 96) {}

void
FACD::Add(uint32_t entity_id, const Body& collision_body)
{
        m_Grid.Insert(entity_id, collision_body.collider.box);
}

void
FACD::Remove(uint32_t entity_id, const Body& collision_body)
{
        m_Grid.Remove(entity_id);
}

void
FACD::Update(uint32_t entity_id, Body& collision_body, const glm::vec2& new_pos,
             const glm::vec2& new_size)
{
        collision_body.Update(new_pos, new_size);
        m_Grid.Update(entity_id, collision_body.collider.box);
}

SmallList<uint32_t>
FACD::GetCollisions(uint32_t entity_id, const Body& body)
{
        SmallList<uint32_t> res;
        m_Grid.GetCollisions(body.collider.box, entity_id, &res);
        return res;
}

SmallList<uint32_t>
FACD::GetCollisions(glm::vec2 position, uint32_t ignore_id)
{
        SmallList<uint32_t> res;
        m_Grid.GetCollisions(position, ignore_id, &res);
        return res;
}

glm::vec2
FACD::MoveAndCollide(uint32_t entity_id, Body& body,
                     float dx, float dy)
{
        auto res = m_Grid.MoveAndCollide(entity_id, dx, dy, body.physics_flags);
        SimdVec4f difference_vec(res.x, res.y, res.x, res.y);
        body.collider.box = simd_add4f(body.collider.box, difference_vec);

        return res;
}

} // namespace FZX
}  // namespace Borek
