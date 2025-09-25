// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Core.h"
#include <cstring>
#include <string>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <Include/Base/Node.h>
#include <Include/Debug/Log.h>
#include <Include/Base/Application.h>

#include "Misc/NodeIcons.h"
#include "ChangeNodeTypePopup.h"

namespace Borek {
namespace Popups {

ChangeNodeTypePopup::ChangeNodeTypePopup(uint32_t parent_id)
        : m_EntityId(parent_id) {}

bool
ChangeNodeTypePopup::Tick()
{
        bool is_open = true;
        ImGui::Begin("Change Node Type", &is_open);

        ImGui::Text("Filter");
        ImGui::SameLine();
        ImGui::InputText("##node_filter", &m_Search);

        for (int i = 0; auto item : node_type_strings) {
                if (!strcasestr(item.c_str(), m_Search.c_str()))
                        continue;

                bool is_selected = m_SelectedItem == i;
                ImGui::Image(get_node_type_icon(SCAST<NodeType>(i))->GetId(),
                             {32, 32}, {0, 1}, {1, 0});
                ImGui::SameLine();
                if (ImGui::Selectable(item.c_str(), is_selected))
                        m_SelectedItem = i;
                if (is_selected)
                        ImGui::SetItemDefaultFocus();

                i++;
        }

        ImGui::Separator();

        if (ImGui::Button("Create")) {
                Application::GetScene()->ChangeEntityNodeType(m_EntityId, SCAST<NodeType>(m_SelectedItem));
                is_open = false;
        }

        ImGui::End();

        return !is_open; 
}

}  // namespace Popups
}  // namespace Borek

