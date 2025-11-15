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
#include <Include/Components/RubyScriptComponent.h>

#include "AddScriptPopup.h"

namespace Borek {
namespace Popups {

AddScriptPopup::AddScriptPopup(uint32_t parent_id)
        : m_EntityId(parent_id) {}

bool
AddScriptPopup::Tick()
{
        bool is_open = true;
        ImGui::Begin("Add Node", &is_open);

        ImGui::Text("Filter");
        ImGui::SameLine();
        ImGui::InputText("##node_filter", &m_Search);

        for (const std::string& cname : Application::GetRubyEngine().GetClasses()) {
                if (!strcasestr(cname.c_str(), m_Search.c_str()))
                        continue;

                bool is_selected = m_SelectedClass == cname;

                if (ImGui::Selectable(cname.c_str(), is_selected))
                        m_SelectedClass = cname;
                if (is_selected)
                        ImGui::SetItemDefaultFocus();
        }

        ImGui::Separator();

        //if (ImGui::Button("Add Script")) {
        //        Entity(m_EntityId).AddComponent<RubyScriptComponent>()
        //                          .GetComponent<RubyScriptComponent>()
        //                          .ruby_class = m_SelectedClass;
        //        is_open = false;
        //}

        ImGui::End();

        return !is_open; 
}

}  // namespace Popups
}  // namespace Borek
