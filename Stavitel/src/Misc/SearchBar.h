// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <iterator>

#include <imgui.h>
#include <imsearch/imsearch.h>

namespace Borek {


template <class T> requires std::random_access_iterator<std::ranges::iterator_t<T>>
void
SearchBar(const T& items, const char** selected_string, const char* tag = "##S")
{
        bool can_select = true;

        if (ImGui::BeginCombo(tag, *selected_string)) {
                if (ImSearch::BeginSearch()) {
                        ImSearch::SearchBar();
                        for (auto str : items) {
                                ImSearch::SearchableItem(str, [&selected_string, &can_select](const char* name) {
                                        const bool is_selected = name == *selected_string;
                                        if (can_select && ImGui::IsKeyPressed(ImGuiKey_Enter)) {
                                                ImGui::CloseCurrentPopup();
                                                *selected_string = name;
                                                can_select = false;
                                        }
                                        if (ImGui::Selectable(name, is_selected)) {
                                                *selected_string = name;
                                        }
                                });
                        }
                        ImSearch::EndSearch();
                }
                ImGui::EndCombo();
        }
}

}  // namespace Borek
