// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Events/Events.h"
#include <imgui.h>

#include <Borek/Include/Base/Application.h>
#include <Include/Base/Components.h>
#include <Include/Core.h>
#include <Include/Debug/Log.h>

#include "Scene.h"

namespace Borek
{
namespace Panels
{

enum EntityFlags {
        kDelete = 1,
};

Scene::Scene()
{
        m_SelectedEntity = Entity();
}

void Scene::OnImguiRender()
{
        if (ImGui::Begin("Scene")) {
                Application::BaseQueries().entity_query.each([this](View& v) {
                        DrawTreeNode(Entity(v.current_entity_id,
                                            Application::GetScene().get()));
                });

                if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
                        m_SelectedEntity = Entity();
                        SendOnSelectionChangeEvent();
                }

                if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                        if (ImGui::MenuItem("Create new Entity"))
                                Application::GetScene()->NewEntity();

                        ImGui::EndPopup();
                }

                ImGui::End();
        };
}

void Scene::DrawTreeNode(Entity e)
{
        uint32_t entity_flags = 0;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_Selected * (m_SelectedEntity == e);
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        flags |= ImGuiTreeNodeFlags_Leaf;

        bool is_open =
                ImGui::TreeNodeEx(VPCAST(e.GetId()), flags, "%s",
                                  e.GetComponent<TagComponent>().value.c_str());

        if (ImGui::IsItemClicked()) {
                if (m_SelectedEntity != e) {
                        m_SelectedEntity = e;
                        SendOnSelectionChangeEvent();
                }
        }

        if (ImGui::BeginPopupContextItem(0)) {
                if (ImGui::MenuItem("Delete Entity"))
                        entity_flags |= kDelete;

                ImGui::EndPopup();
        }

        if (is_open) {
                ImGui::TreePop();
        }

        if (entity_flags & kDelete) {
                if (m_SelectedEntity == e) {
                        m_SelectedEntity = Entity();
                        SendOnSelectionChangeEvent();
                }

                e.Delete();
        }
}

void Scene::SendOnSelectionChangeEvent()
{
        ScenePanelSelectedEvent ev(m_SelectedEntity);
        Application::SendEvent(ev);
}

} // namespace Panels
} // namespace Borek
