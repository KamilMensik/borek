// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/FZX/FACD.h"
#include "Include/Engine/FZX/Body.h"
#include "Include/Platform/SIMD.h"

namespace Borek {
namespace FZX {

FACD::FACD()
        : m_Grid(-1536, -1536, 11520, 1536, 96, 96),
        m_AreaGrid(-1536, -1536, 11520, 1536, 96, 96)
{
}

uint32_t
FACD::Add(uint32_t entity_id, Body& body, const glm::vec2& pos,
          const glm::vec2& size, float rotation)
{
        if (body.body_type == BodyType::Static)
                return m_Grid.Insert(entity_id, body, pos, size, rotation);
        else
                return m_AreaGrid.Insert(entity_id, body, pos, size, rotation);
}

void
FACD::Remove(uint32_t entity_id, const Body& collision_body)
{
        if (collision_body.body_type == BodyType::Static)
                m_Grid.Remove(entity_id);
        else
                m_AreaGrid.Remove(entity_id);
}

void
FACD::RemoveTilemapNode(uint32_t entity_id, uint32_t element_id)
{
        m_Grid.RemoveTilemapNode(entity_id, element_id);
}

void
FACD::Update(uint32_t entity_id, Body& body, const glm::vec2& new_pos,
             const glm::vec2& new_size, float rotation)
{
        if (body.body_type == BodyType::Static)
                m_Grid.Update(entity_id, body, new_pos, new_size, rotation);
        else
                m_AreaGrid.Update(entity_id, body, new_pos, new_size, rotation);
}

SmallList<uint32_t>
FACD::GetCollisions(uint32_t entity_id, const Body& body)
{
        SmallList<uint32_t> res;

        const CGrid::Element* el;
        if (body.body_type == BodyType::Static) {
                el = &m_Grid.GetElement(entity_id);
        } else {
                el = &m_AreaGrid.GetElement(entity_id);
        }

        m_Grid.GetCollisions(*el, entity_id, &res);

        if (body.body_type == BodyType::Area)
                m_AreaGrid.GetCollisions(*el, entity_id, &res);

        return res;
}

SmallList<uint32_t>
FACD::GetCollisions(glm::vec2 position, uint32_t ignore_id, bool ignore_areas)
{
        SmallList<uint32_t> res;
        m_Grid.GetCollisions(position, ignore_id, &res);

        if (!ignore_areas)
                m_AreaGrid.GetCollisions(position, ignore_id, &res);

        return res;
}

std::pair<Collision, glm::vec2>
FACD::MoveAndCollide(uint32_t entity_id, Body& body, float dx, float dy)
{
        const CGrid::Element& el = m_AreaGrid.GetElement(entity_id);
        const auto res = m_Grid.MoveAndCollide(el, dx, dy, body.physics_flags);
        const glm::vec2& pba = res.second;

        const M128 new_aabb = simd_add4f(
                el.aabb, simd_create4f(pba.x, pba.y, pba.x, pba.y));

        m_AreaGrid.Update(entity_id, new_aabb);

        return res;
}

} // namespace FZX
}  // namespace Borek
