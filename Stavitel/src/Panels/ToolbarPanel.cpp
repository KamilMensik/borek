// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "EditorState.h"
#include "Include/Graphics/Texture.h"
#include <imgui.h>
#include <imgui_internal.h>

#include <Borek/Include/Base/Application.h>

#include "./ToolbarPanel.h"

namespace Borek {
namespace Panels {

Ref<Graphics::Texture2D> play_image;
Ref<Graphics::Texture2D> stop_image;

Toolbar::Toolbar()
{
        play_image = Graphics::Texture2D::Create(ASSET_PATH("/assets/EditorIcons/play.png"));
        stop_image = Graphics::Texture2D::Create(ASSET_PATH("/assets/EditorIcons/stop.png"));
}

void Toolbar::OnImguiRender(GizmoPanel& panel)
{
        const ImGuiWindowFlags toolbar_flags = ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoScrollbar |
                ImGuiWindowFlags_NoScrollWithMouse;
        if (ImGui::Begin("##Toolbar", nullptr, toolbar_flags)) {
                float line_height = GImGui->FontSize + GImGui->Style.FramePadding.y * 2;
                ImVec2 button_size = { line_height + 3.0f, line_height + 1.0f };
                if (ImGui::Button("Nothing", button_size))
                        panel.SetMode(Panels::GizmoPanel::Mode::kNothing);
                ImGui::SameLine();
                if (ImGui::Button("Translate", button_size))
                        panel.SetMode(Panels::GizmoPanel::Mode::kTranslate);
                ImGui::SameLine();
                if (ImGui::Button("Rotate", button_size))
                        panel.SetMode(Panels::GizmoPanel::Mode::kRotate);
                ImGui::SameLine();
                if (ImGui::Button("Scale", button_size))
                        panel.SetMode(Panels::GizmoPanel::Mode::kScale);
                ImGui::SameLine();

                bool is_playing = EditorState::game_state == GameState::kPlaying;
                auto tex = is_playing ? stop_image : play_image;
                if (ImGui::ImageButton("Play", tex->GetId(), button_size,
                                       {0, 1}, {1, 0})) {
                        if (is_playing)
                                EditorState::game_state = GameState::kStopped;
                        else
                                EditorState::game_state = GameState::kPlaying;
                }

                ImGui::End();
        }
}

}  // namespace Panels
}  // namespace Borek
