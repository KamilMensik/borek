// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Engine/ZIndexAssigner.h"
#include <glm/common.hpp>
#include <glm/ext/vector_float2.hpp>

#include <Include/Engine/Assets/TilemapAsset.h>
#include <Include/Components/TilemapComponent.h>
#include <Include/Base/Input.h>
#include <Include/Drawing/Quad.h>

#include "Tools/Tilemap/BrushTool.h"

namespace Borek {

void
BrushTool::OnMousePressed(MouseButton button)
{
        if (!(m_Active && !m_CurrentEntity.IsNil() && m_SelectedCell != UINT32_MAX))
                return;

        switch (button) {
        case MouseButton::BUTTON_LEFT:
        case MouseButton::BUTTON_RIGHT:
                break;
        default:
                return;
        }

        const glm::vec2 rel_pos = glm::abs(Input::GetMousePosRelative());
        if (rel_pos.x > 1 || rel_pos.y > 1)
                return;

        m_State = button == MouseButton::BUTTON_LEFT ? State::kDrawing : State::kErasing;
}

void
BrushTool::OnMouseReleased(MouseButton button)
{
        if (!(m_Active && !m_CurrentEntity.IsNil() && m_SelectedCell != UINT32_MAX))
                return;

        switch (button) {
        case MouseButton::BUTTON_LEFT:
                if (m_State == State::kDrawing)
                        m_State = State::kNothing;
                break;
        case MouseButton::BUTTON_RIGHT:
                if (m_State == State::kErasing)
                        m_State = State::kNothing;
                break;
        default:
                break;
        }
}

bool
BrushTool::Tick()
{
        const glm::vec2 rel_pos = glm::abs(Input::GetMousePosRelative());
        if (!(m_Active && !m_CurrentEntity.IsNil() &&
                m_SelectedCell != UINT32_MAX) ||
                (rel_pos.x > 1 || rel_pos.y > 1)) {
                return false;
        }

        auto& tc = m_CurrentEntity.GetComponent<TilemapComponent>();
        Asset<TilemapAsset> tmap = tc.tilemap;
        SpriteSheetAsset& spritesheet = tmap->sprite_sheet.Convert();

        const glm::vec2 scale = m_CurrentEntity.GlobalTransform().scale;
        const glm::vec2 tile_step(tc.step_x * scale.x, tc.step_y * scale.y);
        const glm::vec2 tile_size(spritesheet.step_x * scale.x, spritesheet.step_y * scale.y);
        const glm::vec2 pos = glm::round((Input::GetMouseWorldPos() - tile_step * 0.5f) / tile_step);

        switch (m_State) {
        case State::kNothing:
                Drawing::Quad::Draw(pos * tile_step, tile_size, spritesheet.texture,
                                    Color(1, 1, 1, 0.65), ZIndexAssigner::GetTop(),
                                    spritesheet.SubTextureCords(m_SelectedCell));
                break;
        case State::kDrawing:
                tc.Set(pos.x, pos.y, m_SelectedCell);
                break;
        case State::kErasing:
                tc.Delete(pos.x, pos.y);
                break;
        }

        return true;
}

void
BrushTool::Activate()
{
        m_Active = true;
}

void
BrushTool::Deactivate()
{
        m_Active = false;
}

void
BrushTool::SetData(const ITilemapTool::DataPayload& payload)
{
        m_CurrentEntity = payload.entity;
        m_SelectedCell = payload.selected_cell;
}

}  // namespace Borek
