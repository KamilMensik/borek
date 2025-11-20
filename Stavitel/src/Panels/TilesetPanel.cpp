// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/TilemapComponent.h"
#include "Include/Events/KeyEvents.h"
#include "Include/Events/MouseEvents.h"
#include "Panels/PanelEvents.h"
#include "Tools/Tilemap/BrushTool.h"
#include <cstdint>
#include <imgui.h>

#include "Panels/TilesetPanel.h"
#include "Include/Base/Node.h"
#include "Include/Engine/Assets/TilemapAsset.h"
#include <string>

namespace Borek {
namespace Panels {

Tileset::Tileset()
{
        m_Tools.emplace_back(new BrushTool());
        m_EventHandles[0] = ChangeEntityEvent::AddListener(EVENT_FN(OnChangeEntity));
        m_EventHandles[1] = MouseButtonEvent::AddListener(EVENT_FN(OnMouseButton));
        m_EventHandles[2] = KeyEvent::AddListener(EVENT_FN(OnKey));
}

Tileset::~Tileset()
{
        ChangeEntityEvent::RemoveListener(m_EventHandles[0]);
        MouseButtonEvent::RemoveListener(m_EventHandles[1]);
        KeyEvent::RemoveListener(m_EventHandles[2]);
}

void
Tileset::OnUpdate()
{
        bool block_events = false;
        for (ITilemapTool* tool : m_Tools) {
                block_events |= tool->Tick();
        }

        if (block_events) {
                MouseButtonEvent::Grab(m_EventHandles[1]);
        } else {
                MouseButtonEvent::Release(m_EventHandles[1]);
        }
}

void
Tileset::OnImGuiRender()
{
        if (!ImGui::Begin("Tileset", 0, ImGuiWindowFlags_MenuBar)) {
                ImGui::End();

                for (ITilemapTool* tool : m_Tools) {
                        tool->Deactivate();
                }

                return;
        }

        if (ImGui::BeginMenuBar()) {
                if (ImGui::MenuItem("Nothing", NULL, m_SelectedIndex == UINT32_MAX, true)) {
                        for (ITilemapTool* tool : m_Tools)
                                tool->Deactivate();

                        m_SelectedToolIndex = UINT32_MAX;
                }
                if (ImGui::MenuItem("Brush", NULL, m_SelectedToolIndex == 0)) {
                        m_Tools[0]->Activate();
                        m_SelectedToolIndex = 0;
                }
                ImGui::EndMenuBar();
        }

        if (m_SelectedEntity.IsNil()) {
                ImGui::Text("Select a TileMap node first.");
                ImGui::End();
                return;
        }

        Asset<TilemapAsset>& tmap = m_SelectedEntity.GetComponent<TilemapComponent>().tilemap;
        if (!tmap.IsValid()) {
                ImGui::End();
                return;
        }

        const uint32_t rows = tmap->sprite_sheet->GetRows();
        const uint32_t cols = tmap->sprite_sheet->GetCols();
        if (ImGui::BeginListBox("##ssa", ImVec2(300, 300)))
        {
                std::vector<std::string> rc(rows * cols);
                for (int row = rows - 1; row >= 0; row--) {
                        for (int col = 0; col < cols; col++) {
                                const uint64_t tile_id = col + row * cols;
                                rc[tile_id] = std::to_string(tile_id);
                                const glm::vec4 bounds = tmap->sprite_sheet->SubTextureCords(row, col);
                                if (ImGui::ImageButton(rc[tile_id].c_str(), tmap->sprite_sheet->texture->GetId(),
                                                       ImVec2(tmap->sprite_sheet->step_x * 3, tmap->sprite_sheet->step_y * 3), {bounds.x, bounds.w}, {bounds.z, bounds.y})) {
                                        m_SelectedIndex = tile_id;

                                        ITilemapTool::DataPayload payload(tmap, m_SelectedEntity, tile_id);

                                        for (auto tool : m_Tools) {
                                                tool->SetData(payload);
                                        }
                                }

                                if (col != (cols - 1))
                                        ImGui::SameLine();
                        }
                }

                ImGui::EndListBox();
        }

        if (tmap.IsValid()) {
                const glm::vec4 cords = tmap->sprite_sheet->SubTextureCords(m_SelectedIndex);
                ImGui::Image(tmap->sprite_sheet->texture->GetId(),
                             ImVec2(120, 120), { cords.x, cords.w }, { cords.z, cords.y });
        }

        ImGui::End();
}

void
Tileset::OnChangeEntity(ChangeEntityEvent& e)
{
        if (e.GetEntity().GetNodeType() != NodeType::Tilemap)
                e = Entity();

        m_SelectedEntity = e.GetEntity();
        ITilemapTool::DataPayload payload(Asset<TilemapAsset>(),
                                          m_SelectedEntity, UINT32_MAX);
        for (auto tool : m_Tools) {
                tool->SetData(payload);
        }
}

void
Tileset::OnMouseButton(MouseButtonEvent& ev)
{
        if (m_SelectedEntity.IsNil())
                return;

        auto& tc = m_SelectedEntity.GetComponent<TilemapComponent>();
        if (!tc.tilemap.IsValid())
                return;

        if (ev.IsPressed()) {
                for (ITilemapTool* tool : m_Tools) {
                        tool->OnMousePressed(ev.GetButton());
                }
        } else {
                for (ITilemapTool* tool : m_Tools) {
                        tool->OnMouseReleased(ev.GetButton());
                }
        }
}

void
Tileset::OnKey(KeyEvent& e)
{

        if (e.IsRepeated() || !e.IsPressed())
                return;

        switch(e.GetKeycode()) {
        case KeyCode::B:
                m_Tools[0]->Activate();
                m_SelectedToolIndex = 0;
                break;
        case KeyCode::N:
                for (ITilemapTool* tool : m_Tools)
                        tool->Deactivate();

                m_SelectedToolIndex = UINT32_MAX;
        default:
                break;
        }
}

}  // namespace Panels
}  // namespace Borek
