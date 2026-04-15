// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include "Include/Base/Symbol.h"
#include "Include/Components/PrefabComponent.h"
#include "Include/Events/Event.h"
#include "Panels/PanelEvents.h"
#include "imgui.h"
#include <Include/Core.h>

namespace Borek {
namespace Panels {

class Properties {
public:
        Properties();
        ~Properties();
        void OnImguiRender();
        void OnChangeEntity(ChangeEntityEvent& e);

        template <class T>
        void
        DrawComponent(const char* label, void(*fn)(Entity),
                      bool ignore_pref = true);
private:
        Borek::Entity m_Entity;
        std::array<EVHandle, 2> m_EventHandles;
        static Symbol s_TagInsert;

        static const ImGuiTreeNodeFlags tflags =
                ImGuiTreeNodeFlags_DefaultOpen |
                ImGuiTreeNodeFlags_AllowItemOverlap |
                ImGuiTreeNodeFlags_Framed |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_FramePadding;
};

template <class T>
void
Properties::DrawComponent(const char* label, void(*fn)(Entity), bool ignore_pref)
{
        if (!m_Entity.HasComponent<T>())
                return;
        if (ignore_pref && m_Entity.HasComponent<PrefabComponent>())
                return;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
        ImGui::Separator();
        bool is_open = ImGui::TreeNodeEx(VPCAST(ECS::GetId<T>()), tflags, "%s",
                                         label);
        ImGui::PopStyleVar();

        if (is_open) {
                fn(m_Entity);

                ImGui::TreePop();
        }
}

}  // namespace Panels
}  // namespace Borek
