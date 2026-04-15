// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include "mrbcpp.h"
#include <imgui.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "Popups/MsgConnectionPopup.h"
#include "Misc/SceneTreeExplorer.h"
#include "Include/Base/Application.h"
#include "Include/Base/Scene.h"

namespace Borek {
namespace Popups {

MsgConnectionPopup::MsgConnectionPopup(uint32_t entity_id, MsgConnection* connection)
        : m_Connection(connection), m_Entity(entity_id), m_SelectedEntity(connection->entity_id)
{
        if (m_Connection->rbsym == UINT32_MAX)
                return;

        mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
        m_FunctionName = mrb_sym2name(mrb, m_Connection->rbsym);
}

bool
MsgConnectionPopup::Tick()
{
        Popup::Tick();
        bool is_open = true;

        ImGui::Begin("Message Connection", &is_open);
        ImGui::Text("Filter");
        ImGui::SameLine();
        ImGui::InputText("##node_filter", &m_Search);

        SceneTreeExplorer()
                .Selected(m_SelectedEntity)
                .Search(m_Search)
                .OnClick([this](Entity e){
                        BOREK_ENGINE_INFO("Selecting entity: {}", e.GetId());
                        m_SelectedEntity = e;
                })
                .Draw(0.8f);

        ImGui::Text("FunctionName");
        ImGui::SameLine();
        ImGui::InputText("##function_name_inp", &m_FunctionName);

        ImGui::Separator();

        if (ImGui::Button("Finish")) {
                BOREK_ENGINE_INFO("Selected Entity: {}", m_SelectedEntity.GetId());
                mrb_state* mrb = Application::GetRubyEngine().GetRubyVM();
                m_Connection->entity_id = m_SelectedEntity;
                m_Connection->rbsym = mrb_intern_cstr(mrb, m_FunctionName.c_str());
                is_open = false;
        }

        ImGui::End();

        return !is_open; 
}

}  // namespace Popups
}  // namespace Borek
