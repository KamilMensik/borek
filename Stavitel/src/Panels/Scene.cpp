// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Events/Events.h"
#include "Include/Base/Entity.h"
#include <imgui.h>
#include <imgui_internal.h>

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

Scene::Scene() : m_RootNode(Application::GetScene()->m_RootNode)
{
}

void Scene::OnImguiRender()
{
        if (ImGui::Begin("Scene")) {
                DrawTreeNode(m_RootNode);

                if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {
                        m_SelectedEntity = nullptr;
                        SendOnSelectionChangeEvent();
                }

                if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                        if (ImGui::MenuItem("Create Entity")) {
                                Entity e = Application::GetScene()->NewEntity();
                                m_RootNode->ChildPrepend(new Node(e));
                        }

                        ImGui::EndPopup();
                }

                ImGui::End();
        };
}

void Scene::DrawTreeNode(Node* node)
{
        Entity e = node->entity;
        uint32_t entity_flags = 0;

        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
        flags |= ImGuiTreeNodeFlags_Selected * (m_SelectedEntity == node);
        flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
        if (node->first_child == nullptr)
                flags |= ImGuiTreeNodeFlags_Leaf;
        flags |= ImGuiTreeNodeFlags_DefaultOpen;


        if (node->parent) {
                ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(1, 1, 1, 1));
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_SpanAllColumns, 2.0f);
                if (ImGui::BeginDragDropTarget()) {
                        auto payload = ImGui::AcceptDragDropPayload("Node");
                        if (payload) {
                                Node* n = *RCAST<Node**>(payload->Data);
                                n->MoveBack(node);
                                ImGui::EndDragDropTarget();
                        }
                }
                ImGui::PopStyleColor(3);
        }

        bool is_open =
                ImGui::TreeNodeEx(VPCAST(e.GetUUID()), flags, "%s (%u)",
                                  e.GetComponent<TagComponent>().value.c_str(),
                                  e.GetId());

        if (node->parent) {
                if (ImGui::BeginDragDropSource()) {
                        ImGui::SetDragDropPayload("Node", &node, sizeof(Node*));
                        ImGui::Text("%s", e.GetName());
                        ImGui::EndDragDropSource();
                }
        }

        if (ImGui::BeginDragDropTarget()) {
                auto payload = ImGui::AcceptDragDropPayload("Node");
                if (payload) {
                        Node* n = *RCAST<Node**>(payload->Data);
                        n->Reparent(node);
                        ImGui::EndDragDropTarget();
                }
        }

        if (ImGui::IsItemClicked()) {
                if (m_SelectedEntity != node) {
                        m_SelectedEntity = node;
                        SendOnSelectionChangeEvent();
                }
        }

        if (ImGui::BeginPopupContextItem(0)) {
                if (node->parent && ImGui::MenuItem("Delete Entity"))
                        entity_flags |= kDelete;
                if (ImGui::MenuItem("Add Entity")) {
                        Entity e = Application::GetScene()->NewEntity();
                        node->ChildPrepend(new Node(e));
                }

                ImGui::EndPopup();
        }

        if (is_open) {
                node->EachChildReverse([this](Node* n){
                        DrawTreeNode(n);
                });
                ImGui::TreePop();
        }

        if (node->parent && !node->prev){
                ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(1, 1, 1, 1));
                ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(1, 1, 1, 1));
                ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_SpanAllColumns, 2.0f);
                if (ImGui::BeginDragDropTarget()) {
                        auto payload = ImGui::AcceptDragDropPayload("Node");
                        if (payload) {
                                Node* n = *RCAST<Node**>(payload->Data);
                                n->Move(node);
                                ImGui::EndDragDropTarget();
                        }
                }
                ImGui::PopStyleColor(3);
        }

        if (entity_flags & kDelete) {
                if (m_SelectedEntity == node) {
                        m_SelectedEntity = nullptr;
                        SendOnSelectionChangeEvent();
                }

                node->Reparent(nullptr);
                node->Delete();
        }
}

void Scene::SendOnSelectionChangeEvent()
{
        Application::SendEvent(new ScenePanelSelectedEvent(m_SelectedEntity? m_SelectedEntity->entity : Entity()));
}

} // namespace Panels
} // namespace Borek
