// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Popups/AddScriptPopup.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <Borek/Include/Base/Application.h>
#include <Borek/Include/Base/Components.h>
#include <Borek/Include/Core.h>
#include <Borek/Include/Debug/Log.h>
#include <Borek/Include/Base/Entity.h>

#include "Scene.h"
#include "Popups/AddEntityPopup.h"
#include "Popups/ChangeNodeTypePopup.h"
#include "Misc/NodeIcons.h"
#include "Events/Events.h"

namespace Borek
{
namespace Panels
{

enum EntityFlags {
        kDelete = 1,
};

static Entity node_payload;
static std::vector<bool> tree_nodes_open_state;

Scene::Scene()
{
}

void Scene::OnImguiRender()
{
        Ref<Borek::Scene> scene = Application::GetScene();
        if (ImGui::Begin("Scene")) {
                ImGui::Text("Search:");
                if (!m_SelectedEntity.IsNil()) {
                        ImGui::SameLine();
                        if (m_SelectedEntity.HasComponent<RubyScriptComponent>()) {
                                if (ImGui::Button("x")) {
                                        m_SelectedEntity.RemoveComponent<RubyScriptComponent>();
                                }
                        } else {
                                if (ImGui::Button("O")) {
                                        Application::OpenPopup(new Popups::AddScriptPopup(m_SelectedEntity));
                                }
                        }
                }

                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
                ImGui::Separator();
                scene->TraverseSceneReverse([this](Entity e){ EntityTreeNodeBegin(e); },
                                            [this](Entity e){ EntityTreeNodeEnd(e); });
                ImGui::PopFont();

                if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
                        m_SelectedEntity = Entity();
                        SendOnSelectionChangeEvent();
                }

                if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                        if (ImGui::MenuItem("Create Entity")) {
                                Application::OpenPopup(new Popups::AddEntityPopup(Application::GetScene()->RootEntity()));
                        }

                        ImGui::EndPopup();
                }

                ImGui::End();
        };
}

void
Scene::EntityTreeNodeBegin(Entity e)
{
        if (!m_Skip.IsNil())
                return;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_Selected * (m_SelectedEntity == e);
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        if (!e.HasChildren())
                flags |= ImGuiTreeNodeFlags_Leaf;
        flags |= ImGuiTreeNodeFlags_DefaultOpen;


        if (e.HasParent()) {
                ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(1, 1, 1, 1));
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_SpanAllColumns, 2.0f);
                if (ImGui::BeginDragDropTarget()) {
                        auto payload = ImGui::AcceptDragDropPayload("Node");
                        if (payload) {

                                Entity p_ent = *RCAST<Entity*>(payload->Data);
                                Application::GetScene()->EntityAppend(p_ent, e);
                                ImGui::EndDragDropTarget();
                        }
                }
                ImGui::PopStyleColor(3);
        }

        bool is_open = ImGui::TreeNodeEx(VPCAST(e.GetUUID()), flags, "");
        tree_nodes_open_state.emplace_back(is_open);
        if (!is_open)
                m_Skip = e;

        if (e.HasParent()) {
                if (ImGui::BeginDragDropSource()) {
                        node_payload = e;
                        ImGui::SetDragDropPayload("Node", &node_payload,
                                                  sizeof(Entity*));
                        ImGui::Text("%s", e.GetName());
                        ImGui::EndDragDropSource();
                }
        }

        if (ImGui::BeginDragDropTarget()) {
                auto payload = ImGui::AcceptDragDropPayload("Node");
                if (payload) {
                        Entity p_ent = *RCAST<Entity*>(payload->Data);
                        Application::GetScene()->EntityAppendChild(p_ent, e);
                        ImGui::EndDragDropTarget();
                }
        }

        if (ImGui::IsItemClicked()) {
                if (m_SelectedEntity != e) {
                        m_SelectedEntity = e;
                        SendOnSelectionChangeEvent();
                }
        }

        if (ImGui::BeginPopupContextItem(0)) {
                if (e.HasParent() && ImGui::MenuItem("Delete Entity")) 
                        Application::SendEvent(new RemoveEntityEvent(e));
                if (ImGui::MenuItem("Add Entity")) {
                        Application::OpenPopup(new Popups::AddEntityPopup(e));
                }
                if (ImGui::MenuItem("Change Node Type")) {
                        Application::OpenPopup(new Popups::ChangeNodeTypePopup(e));
                }

                ImGui::EndPopup();
        }

        ImGui::SameLine();
        ImGui::Image(get_node_type_icon(e.GetNodeType())->GetId(),
                     {32, 32}, {0, 1}, {1, 0});
        ImGui::SameLine();
        ImGui::Text("%s (%d)", e.GetComponent<TagComponent>().value.c_str(),
                    e.m_Id);
}

void Scene::EntityTreeNodeEnd(Entity e)
{
        if (!m_Skip.IsNil()) {
                if (m_Skip == e)
                        m_Skip = Entity();
                else
                        return;
        }

        bool is_open = tree_nodes_open_state.back();
        tree_nodes_open_state.pop_back();

        if (is_open)
                ImGui::TreePop();

        if (e.HasParent()){
                ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(1, 1, 1, 1));
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_SpanAllColumns, 2.0f);
                if (ImGui::BeginDragDropTarget()) {
                        auto payload = ImGui::AcceptDragDropPayload("Node");
                        if (payload) {
                                Entity p_ent = *RCAST<Entity*>(payload->Data);
                                Application::GetScene()->EntityPrepend(p_ent, e);
                                ImGui::EndDragDropTarget();
                        }
                }
                ImGui::PopStyleColor(3);
        }
}

void Scene::SendOnSelectionChangeEvent()
{
        Application::SendEvent(new ScenePanelSelectedEvent(m_SelectedEntity));
}

void
Scene::SetSelectedEntity(Entity e)
{
        m_SelectedEntity = e;
        SendOnSelectionChangeEvent();
}


} // namespace Panels
} // namespace Borek
