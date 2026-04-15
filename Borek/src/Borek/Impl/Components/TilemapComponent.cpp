// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/TilemapComponent.h"
#include "Include/Base/Application.h"
#include "Include/Engine/FZX/Body.h"

namespace Borek {

void
TilemapComponent::Set(int16_t col, int16_t row, uint16_t item)
{
        items[{row, col}] = item;
}

void
TilemapComponent::Delete(int16_t col, int16_t row)
{
        items.erase({row, col});
}

void
TilemapComponent::Set(uint32_t entity_id, int16_t col, int16_t row, uint16_t item)
{
        uint16_t old_item = UINT16_MAX;
        auto it = items.find({row, col});

        if (it != items.end())
                old_item = it->second;

        items[{row, col}] = item;

        if (!(Application::IsPlaying() && tilemap->tile_coliders[item].Exists()))
                return;
        if (tilemap->tile_coliders[old_item].Exists())
                return;

        glm::vec2 scale = Entity(entity_id).GlobalTransform().scale;
        const glm::vec2 real_scale(tilemap->sprite_sheet->step_x * scale.x,
                                   tilemap->sprite_sheet->step_y * scale.y);

        const glm::vec2 real_pos(row * step_x * scale.x, col * step_y * scale.y);
        FZX::Body body;
        body.tc.row = row;
        body.tc.col = col;

        auto& fzxw = Application::GetScene()->m_PhysicsWorld;
        const uint32_t cid = fzxw.Add(entity_id, body, real_pos, real_scale);
        existing_colliders[{row, col}] = cid;
}

void
TilemapComponent::Delete(uint32_t entity_id, int16_t col, int16_t row)
{
        auto it = items.find({row, col});
        if (it == items.end())
                return;

        uint16_t item = it->second;
        items.erase(it);

        if (!(Application::IsPlaying() && tilemap->tile_coliders[item].Exists()))
                return;

        auto& fzxw = Application::GetScene()->m_PhysicsWorld;
        fzxw.RemoveTilemapNode(entity_id, existing_colliders[{row, col}]);
}

}  // namespace Borek
