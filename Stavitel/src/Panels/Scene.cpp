// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/PrefabComponent.h"
#include "Panels/PanelEvents.h"
#include "Popups/AddScriptPopup.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <Borek/Include/Base/Application.h>
#include <Borek/Include/Core.h>
#include <Borek/Include/Debug/Log.h>
#include <Borek/Include/Base/Entity.h>
#include <Borek/Include/Components/TagComponent.h>
#include <Borek/Include/Components/RubyScriptComponent.h>
#include <Borek/Include/Drawing/DrawingGlobals.h>

#include "Scene.h"
#include "Popups/AddEntityPopup.h"
#include "Popups/ChangeNodeTypePopup.h"
#include "Misc/NodeIcons.h"
#include "Events/Events.h"
#include <ranges>

namespace Borek
{
namespace Panels
{

enum EntityFlags {
        kDelete = 1,
};

static Entity node_payload;
static std::vector<bool> tree_nodes_open_state;

static bool
valid_payload(Entity e, Entity p)
{
        return e != p && !p.IsParentOf(e);
}

static void
add_tree_drag_drop(Entity e)
{
        const float tree_node_size = ImGui::GetContentRegionAvail().x;
        const ImVec2 old_pos = ImGui::GetCursorPos();

        ImGui::SetCursorPos(ImVec2(0, old_pos.y - 34));
        ImGui::Image(Drawing::Globals::GetData().white_tex->GetId(),
                     ImVec2(tree_node_size, 8), { 0, 0}, {1, 1},
                     ImVec4(), ImVec4());
        if (e.HasParent() && ImGui::BeginDragDropTarget()) {
                auto payload = ImGui::AcceptDragDropPayload("Node", ImGuiDragDropFlags_AcceptPeekOnly);
                if (payload) {
                        Entity p_ent = *RCAST<Entity*>(payload->Data);
                        if (valid_payload(e, p_ent) && ImGui::AcceptDragDropPayload("Node")) {
                                Application::GetScene()->EntityAppend(p_ent, e);
                                ImGui::EndDragDropTarget();
                        }
                }
        }

        ImGui::SetCursorPos(ImVec2(0, old_pos.y - 26));
        ImGui::Image(Drawing::Globals::GetData().white_tex->GetId(),
                     ImVec2(tree_node_size, 20), { 0, 0}, {1, 1},
                     ImVec4(), ImVec4());
        if (ImGui::BeginDragDropTarget()) {
                auto payload = ImGui::AcceptDragDropPayload("Node", ImGuiDragDropFlags_AcceptPeekOnly);
                if (payload) {
                        Entity p_ent = *RCAST<Entity*>(payload->Data);
                        if (valid_payload(e, p_ent) && ImGui::AcceptDragDropPayload("Node")) {
                                Application::GetScene()->EntityPrependChild(p_ent, e);
                                ImGui::EndDragDropTarget();
                        }
                }
        }

        ImGui::SetCursorPos(ImVec2(0, old_pos.y - 6));
        ImGui::Image(Drawing::Globals::GetData().white_tex->GetId(),
                     ImVec2(tree_node_size, 8), { 0, 0}, {1, 1},
                     ImVec4(), ImVec4());
        if (e.HasParent() && ImGui::BeginDragDropTarget()) {
                auto payload = ImGui::AcceptDragDropPayload("Node", ImGuiDragDropFlags_AcceptPeekOnly);
                if (payload) {
                        Entity p_ent = *RCAST<Entity*>(payload->Data);
                        if (valid_payload(e, p_ent) && ImGui::AcceptDragDropPayload("Node")) {
                                Application::GetScene()->EntityPrepend(p_ent, e);
                                ImGui::EndDragDropTarget();
                        }
                }
        }

        ImGui::SetCursorPos(old_pos);
}

static void
add_invisible_separator()
{
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, ImVec4(1, 1, 1, 1));
        ImGui::PushStyleColor(ImGuiCol_SeparatorActive, ImVec4(1, 1, 1, 1));
        ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal | ImGuiSeparatorFlags_SpanAllColumns, 2.0f);
        ImGui::PopStyleColor(3);
}

Scene::Scene()
{
        m_EventHandles[0] = ChangeEntityEvent::AddListener(EVENT_FN(OnChangeEntity));
}

Scene::~Scene()
{
        ChangeEntityEvent::RemoveListener(m_EventHandles[0]);
}

void Scene::OnImguiRender()
{
        Ref<Borek::Scene> scene = Application::GetScene();
        m_FlatGraph.clear();
        scene->TraverseSceneReverse([this](Entity e) { Phase1IterFuncBegin(e); },
                                    [this](Entity e) { Phase1IterFuncEnd(e); });


        if (ImGui::Begin("Scene")) {
                ImGui::Text("Search:");
                ImGui::SameLine();
                ImGui::InputText("##", &m_Search);

                if (!m_SelectedEntity.IsNil()) {
                        ImGui::SameLine();
                        if (m_SelectedEntity.HasComponent<RubyScriptComponent>()) {
                                if (ImGui::Button("x")) {
                                        m_SelectedEntity.RemoveComponent<RubyScriptComponent>();
                                }
                        } else {
                                if (ImGui::Button("O")) {
                                        m_SelectedEntity.AddComponent<RubyScriptComponent>();
                                        Application::OpenPopup(new Popups::AddScriptPopup(m_SelectedEntity));
                                }
                        }
                }

                Phase2();
                DrawTree();

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
Scene::Phase1IterFuncBegin(Entity e)
{
        if (!m_Skip.IsNil())
                return;

        Operation op;
        if (e.HasComponent<PrefabComponent>()) {
                op = Operation::kKeep;
                m_Skip = e;
        } else {
                op = e.HasChildren() ? Operation::kOpen : Operation::kKeep;
        }

        m_FlatGraph.emplace_back(op, e);
}

void Scene::Phase1IterFuncEnd(Entity e)
{
        if (!m_Skip.IsNil() && m_Skip != e)
                return;

        if (e != m_FlatGraph.back().second && e.HasChildren())
                m_FlatGraph.back().first = Operation::kClose;

        m_Skip = Entity();
}

void
Scene::Phase2()
{
        m_VisibleTreeNodes.clear();
        for (auto [op, e] : std::ranges::views::reverse(m_FlatGraph)) {
                if (m_VisibleTreeNodes.contains(e)) continue;
                if(strcasestr(e.GetName(), m_Search.c_str()) == NULL) continue;

                m_VisibleTreeNodes.insert(e);
                Entity parent = e.GetParent();
                while (!parent.IsNil()) {
                        if (m_VisibleTreeNodes.contains(parent))
                                break;

                        m_VisibleTreeNodes.insert(parent);
                        parent = parent.GetParent();
                }
        }
}

void
Scene::DrawTree()
{
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        ImGui::Separator();

        const ImGuiTreeNodeFlags base_flags =
                ImGuiTreeNodeFlags_OpenOnArrow |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_DefaultOpen;

        int skip_depth = 0;
        bool pop_after_skip = false;
        int depth = 0;

        for (auto& [op, e] : m_FlatGraph) {
                const bool is_prefab = e.HasComponent<PrefabComponent>();
                const bool has_children = e.HasChildren();

                if (skip_depth != 0 || !m_VisibleTreeNodes.contains(e)) {
                        if (op == Operation::kOpen) skip_depth += 1;
                        else if (op == Operation::kClose) skip_depth -= 1;

                        if (skip_depth == 1 && pop_after_skip) {
                                pop_after_skip = false;
                                ImGui::TreePop();
                                depth--;
                        }

                        continue;
                }

                ImGuiTreeNodeFlags flags = base_flags;
                flags |= ImGuiTreeNodeFlags_Selected * (m_SelectedEntity == e);
                flags |= ImGuiTreeNodeFlags_Leaf * (!has_children || is_prefab);

                add_invisible_separator();

                bool is_open = ImGui::TreeNodeEx(VPCAST(e.GetUUID()), flags, "%s", "");
                if (op == Operation::kOpen) {
                        if (is_open) {
                                depth++;
                        } else {
                                skip_depth = 1;
                                pop_after_skip = true;
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
                                Application::SendEvent<RemoveEntityEvent>(e);
                        if (ImGui::MenuItem("Add Entity")) {
                                Application::OpenPopup(new Popups::AddEntityPopup(e));
                        }
                        if (ImGui::MenuItem("Change Node Type")) {
                                Application::OpenPopup(new Popups::ChangeNodeTypePopup(e));
                        }

                        ImGui::EndPopup();
                }

                if (e.HasParent()) {
                        if (ImGui::BeginDragDropSource()) {
                                node_payload = e;
                                ImGui::SetDragDropPayload("Node", &node_payload,
                                                          sizeof(Entity*));
                                ImGui::Text("%s", e.GetName());
                                ImGui::EndDragDropSource();
                        }
                }

                ImGui::SameLine();
                ImGui::Image(get_node_type_icon(e.GetNodeType())->GetId(),
                             {32, 32}, {0, 1}, {1, 0});
                ImGui::SameLine();
                ImGui::Text("%s%s (%d)", is_prefab ? "(P) " : "",
                            e.GetComponent<TagComponent>().value.c_str(),
                            e.GetId());

                add_tree_drag_drop(e);

                if (op == Operation::kKeep) {
                        ImGui::TreePop();
                } else if (op == Operation::kClose) {
                        ImGui::TreePop();
                        ImGui::TreePop();
                        depth -= 1;
                }
        }

        for (int i = 0; i < depth; i++) {
                ImGui::TreePop();
        }

        ImGui::PopFont();
}

void Scene::SendOnSelectionChangeEvent()
{
        Application::SendEvent<ScenePanelSelectedEvent>(m_SelectedEntity);
}

void
Scene::OnChangeEntity(ChangeEntityEvent& e)
{
        m_SelectedEntity = e.GetEntity();
}


} // namespace Panels
} // namespace Borek
