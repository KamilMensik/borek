// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "EditorSettings.h"
#include "Include/Base/Application.h"
#include "Include/Events/MouseEvents.h"
#include "Panels/PanelEvents.h"
#include <imgui.h>

#include "Panels/Toolbar.h"
#include "Include/Core.h"
#include "Tools/MoveTool.h"
#include "Misc/FontAwesome.h"

namespace Borek {
namespace Panels {

Toolbar::Toolbar()
{
        m_Tools.emplace_back(NewUniq<MoveTool>());
        m_EventHandles[0] = ChangeEntityEvent::AddListener(EVENT_FN(OnChangeEntity));
        m_EventHandles[1] = MouseButtonEvent::AddListener(EVENT_FN(OnMouseButton));
}

Toolbar::~Toolbar()
{
        ChangeEntityEvent::RemoveListener(m_EventHandles[0]);
}

void
Toolbar::Tick()
{
        static bool grabbed_events = false;
        bool active = false;

        for (auto& tool : m_Tools) {
                active |= tool->Tick();
        }

        if (active && !grabbed_events) {
                MouseButtonEvent::Grab(m_EventHandles[1]);
                grabbed_events = true;
        } else if (!active && grabbed_events) {
                MouseButtonEvent::Release(m_EventHandles[1]);
                grabbed_events = false;
        }
}

void
Toolbar::OnImguiRender()
{
        if (Application::IsPlaying())
                return;

        if (!ImGui::BeginTabBar("Toolbar"))
                return;

        if (ImGui::BeginTabItem(ICON_FA_ARROW_POINTER)) {
                m_Tools[0]->Deactivate();
                ImGui::EndTabItem();
        }
        ImGui::SetItemTooltip("No Tool");

        if (ImGui::BeginTabItem(ICON_FA_UP_DOWN_LEFT_RIGHT)) {
                m_Tools[0]->Activate();
                ImGui::EndTabItem();
        }
        ImGui::SetItemTooltip("Move Tool");

        ImGui::SameLine();
        ImGui::PushItemWidth(75);
        ImGui::Text(ICON_FA_TABLE_CELLS_LARGE);
        ImGui::SetItemTooltip("Grid Snap");
        ImGui::SameLine();
        ImGui::InputScalar("##SNAP_X", ImGuiDataType_U32,
                           &EditorSettings::grid_snap.first);

        ImGui::SameLine();
        ImGui::InputScalar("##SNAP_Y", ImGuiDataType_U32,
                           &EditorSettings::grid_snap.second);
        ImGui::PopItemWidth();

        ImGui::EndTabBar();
}

void
Toolbar::OnChangeEntity(ChangeEntityEvent& ev)
{
        for (auto& tool : m_Tools) {
                tool->SetEntity(ev.GetEntity());
        }
}

void
Toolbar::OnMouseButton(MouseButtonEvent& ev)
{
        if (ev.IsPressed()) {
                for (auto& tool : m_Tools) {
                        tool->OnMousePressed(ev.GetButton());
                }

                return;
        }

        for (auto& tool : m_Tools) {
                tool->OnMouseReleased(ev.GetButton());
        }
}

}  // namespace Panels
}  // namespace Borek
