// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "EditorState.h"
#include "Include/Engine/Assets/AssetManager.h"
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
        play_image = AssetManager::GetTextureRaw(ASSET_PATH("assets/EditorIcons/play.png"));
        stop_image = AssetManager::GetTextureRaw(ASSET_PATH("assets/EditorIcons/stop.png"));
}

void Toolbar::OnImguiRender(GizmoPanel& panel)
{
        bool is_playing = EditorState::game_state == GameState::kPlaying;
        auto tex = is_playing ? stop_image : play_image;

        if (ImGui::BeginMenuBar()) {
                if (ImGui::MenuItem("Nothing"))
                        panel.SetMode(Panels::GizmoPanel::Mode::kNothing);
                if (ImGui::MenuItem("Move"))
                        panel.SetMode(Panels::GizmoPanel::Mode::kTranslate);
                if (ImGui::MenuItem("Rotate"))
                        panel.SetMode(Panels::GizmoPanel::Mode::kRotate);
                if (ImGui::MenuItem("Scale"))
                        panel.SetMode(Panels::GizmoPanel::Mode::kScale);
                if (ImGui::ImageButton("Play", tex->GetId(), ImVec2(20, 20),
                                       {0, 1}, {1, 0})) {
                        if (is_playing)
                                EditorState::game_state = GameState::kStopped;
                        else
                                EditorState::game_state = GameState::kPlaying;
                }

                ImGui::EndMenuBar();
        }
}

}  // namespace Panels
}  // namespace Borek
