// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <array>
#include <imgui.h>
#include <imsearch/imsearch.h>
#include <imsearch/imsearch_internal.h>

#include "Panels/TilesetPanel.h"

namespace Borek {
namespace Panels {

void
Tileset::OnImGuiRender()
{
        static std::array<const char*, 3> test = { "Ahoj", "Jak se mas", "Tady je jirka" };
        static const char* selected_string = test[0];
        static bool can_select = true;

        if (ImGui::Begin("Tileset", 0, ImGuiWindowFlags_MenuBar)) {
                if (ImGui::BeginCombo("##SelectedTileset", selected_string)) {
                        if (ImSearch::BeginSearch()) {
                                ImSearch::SearchBar();
                                for (auto str : test) {
                                        ImSearch::SearchableItem(str, [](const char* name) {
                                                const bool is_selected = name == selected_string;
                                                if (can_select && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                                                        ImGui::CloseCurrentPopup();
                                                        selected_string = name;
                                                        can_select = false;
                                                }
                                                if (ImGui::Selectable(name, is_selected)) {
                                                        selected_string = name;
                                                }
                                        });
                                }
                                ImSearch::EndSearch();
                        }
                        ImGui::EndCombo();
                }
        }

        can_select = true;

        ImGui::End();
}


}  // namespace Panels
}  // namespace Borek
