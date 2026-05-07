// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <filesystem>
#include <ranges>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include "Include/Drawing/DrawingGlobals.h"
#include "Include/Components/RubyScriptComponent.h"
#include "Include/Base/Application.h"
#include "Include/Base/Scene.h"
#include "Include/Components/PrefabComponent.h"

#include "Misc/SceneTreeExplorer.h"
#include "Misc/SceneTabBar.h"
#include "Misc/NodeIcons.h"
#include "Commands/EntityCommands.h"
#include "Misc/FontAwesome.h"

namespace Borek {

static RelatedEntity
get_safe_prepend_related_entity(Entity e)
{
        Entity prev = e.GetNextSibling();
        if (prev.IsNil())
                return { e.GetParent(), true };

        return { prev, false };
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

static bool
valid_payload(Entity e, Entity p)
{
        return e != p && !p.IsParentOf(e);
}

static Entity
get_drag_drop_entity(Entity parent)
{
        auto payload = ImGui::AcceptDragDropPayload("Node", ImGuiDragDropFlags_AcceptPeekOnly);
        if (payload) {
                Entity p_ent = *RCAST<Entity*>(payload->Data);
                if (valid_payload(parent, p_ent) && ImGui::AcceptDragDropPayload("Node")) {
                        return p_ent;
                }
        }
        payload = ImGui::AcceptDragDropPayload("FILE_EXPL_ITEM", ImGuiDragDropFlags_AcceptPeekOnly);
        if (payload) {
                const auto p = Utils::Path::ToRelative(RCAST<const char*>(payload->Data));
                if (p.extension() == ".scn" && ImGui::AcceptDragDropPayload("FILE_EXPL_ITEM")) {
                        return Application::GetScene()->CreateFromPrefab(p.string());
                }
        }

        return Entity();
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
                Entity p_ent = get_drag_drop_entity(e);
                if (!p_ent.IsNil())
                        SceneTabBar::SendCommand<MoveEntityCommand>(
                                p_ent, get_safe_prepend_related_entity(e));

                ImGui::EndDragDropTarget();
        }

        ImGui::SetCursorPos(ImVec2(0, old_pos.y - 26));
        ImGui::Image(Drawing::Globals::GetData().white_tex->GetId(),
                     ImVec2(tree_node_size, 20), { 0, 0}, {1, 1},
                     ImVec4(), ImVec4());
        if (!e.HasComponent<PrefabComponent>() && ImGui::BeginDragDropTarget()) {
                Entity p_ent = get_drag_drop_entity(e);
                if (!p_ent.IsNil())
                        SceneTabBar::SendCommand<MoveEntityCommand>(
                                p_ent, RelatedEntity(e, true));

                ImGui::EndDragDropTarget();
        }

        ImGui::SetCursorPos(ImVec2(0, old_pos.y - 6));
        ImGui::Image(Drawing::Globals::GetData().white_tex->GetId(),
                     ImVec2(tree_node_size, 8), { 0, 0}, {1, 1},
                     ImVec4(), ImVec4());

        if (e.HasParent() && ImGui::BeginDragDropTarget()) {
                Entity p_ent = get_drag_drop_entity(e);
                if (!p_ent.IsNil())
                        SceneTabBar::SendCommand<MoveEntityCommand>(
                                p_ent, RelatedEntity(e, false));

                ImGui::EndDragDropTarget();
        }

        ImGui::SetCursorPos(old_pos);
}

static Entity node_payload;

SceneTreeExplorer&
SceneTreeExplorer::Search(std::string_view value) {
        m_Search = value;
        return *this;
}

SceneTreeExplorer&
SceneTreeExplorer::OnClick(std::function<void(Entity)> callback)
{
        m_OnClickCallback = callback;
        return *this;
}

SceneTreeExplorer&
SceneTreeExplorer::OnRightClick(std::function<void(Entity)> callback)
{
        m_OnRightClickCallback = callback;
        return *this;
}

SceneTreeExplorer&
SceneTreeExplorer::Selected(Entity e)
{
        m_SelectedEntity = e;
        return *this;
}

SceneTreeExplorer&
SceneTreeExplorer::AllowReorder()
{
        m_AllowReorder = true;
        return *this;
}

SceneTreeExplorer&
SceneTreeExplorer::ContextMenu(std::function<void(Entity)> callback)
{
        m_ContextMenu = callback;
        m_AllowContextMenu = true;
        return *this;
}

void
SceneTreeExplorer::Draw(float y_percent)
{
        Ref<Scene> scene = Application::GetScene();
        m_FlatGraph.clear();
        scene->GetTree().TraverseSceneReverse([this](Entity e) { Phase1IterFuncBegin(e); },
                                              [this](Entity e) { Phase1IterFuncEnd(e); });

        Phase2();

        ImVec2 space = ImGui::GetContentRegionAvail();
        space.y *= y_percent;

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        if (ImGui::BeginChild("stree", space)) {
                DrawTree();

                if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered())
                        m_OnClickCallback(Entity());

                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
                        m_OnRightClickCallback(Entity());

                if (m_AllowContextMenu &&
                    ImGui::BeginPopupContextWindow("cntxt", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
                        m_ContextMenu(Entity());
                        ImGui::EndPopup();
                }
        }
        ImGui::EndChild();
        ImGui::PopFont();
}

void
SceneTreeExplorer::Phase1IterFuncBegin(Entity e)
{
        if (!m_Skip.IsNil())
                return;

        if (e.HasComponent<PrefabComponent>())
                m_Skip = e;
        

        m_FlatGraph.emplace_back(1, e);
}

void
SceneTreeExplorer::Phase1IterFuncEnd(Entity e)
{
        if (!m_Skip.IsNil() && m_Skip != e)
                return;

        --m_FlatGraph.back().first;

        m_Skip = Entity();
}

void
SceneTreeExplorer::Phase2()
{
        m_VisibleTreeNodes.clear();
        for (auto [op, e] : std::ranges::views::reverse(m_FlatGraph)) {
                if (m_VisibleTreeNodes.contains(e)) continue;
                if(strcasestr(e.GetName(), m_Search.data()) == NULL) continue;

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
SceneTreeExplorer::DrawTree()
{
        ImGui::Separator();

        const ImGuiTreeNodeFlags base_flags =
                ImGuiTreeNodeFlags_OpenOnArrow |
                ImGuiTreeNodeFlags_SpanAvailWidth |
                ImGuiTreeNodeFlags_AllowOverlap |
                ImGuiTreeNodeFlags_DefaultOpen;

        int skip_depth = 0;
        bool pop_after_skip = false;

        for (auto& [op, e] : m_FlatGraph) {
                const bool is_prefab = e.HasComponent<PrefabComponent>();
                const bool has_script = e.HasComponent<RubyScriptComponent>();
                const bool has_children = e.HasChildren();

                if (skip_depth != 0 || !m_VisibleTreeNodes.contains(e)) {
                        skip_depth += op;

                        if (skip_depth == 0 && pop_after_skip) {
                                pop_after_skip = false;
                        }
                        
                        for (int i = 0 - skip_depth; i > 0; i--) {
                                ImGui::TreePop();
                        }

                        continue;
                }

                ImGuiTreeNodeFlags flags = base_flags;
                flags |= ImGuiTreeNodeFlags_Selected * (m_SelectedEntity == e);
                flags |= ImGuiTreeNodeFlags_Leaf * (!has_children || is_prefab);

                add_invisible_separator();

                bool is_open = ImGui::TreeNodeEx(VPCAST(e.GetId()), flags, "%s", "");
                if (op == 1) {
                        if (!is_open) {
                                skip_depth = 1;
                                pop_after_skip = true;
                        }
                }

                if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                        m_OnClickCallback(e);

                if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
                        m_OnRightClickCallback(e);

                if (m_AllowContextMenu &&
                    ImGui::BeginPopupContextItem("cntxt", ImGuiMouseButton_Right)) {
                        m_ContextMenu(e);
                        ImGui::EndPopup();
                }

                if (ImGui::BeginDragDropSource()) {
                        node_payload = e;
                        ImGui::SetDragDropPayload("Node", &node_payload,
                                                  sizeof(Entity*));
                        ImGui::Text("%s", e.GetName());
                        ImGui::EndDragDropSource();
                }

                ImGui::SameLine();
                ImGui::Text("%s %s", get_node_type_icon(e.GetNodeType()), e.GetName());

                float pos_x = -1.0f;
                if (is_prefab) {
                        ImGui::SameLine();
                        pos_x = ImMax(0.0f, ImGui::GetContentRegionAvail().x);
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + pos_x - ImGui::CalcTextSize(ICON_FA_CUBE).x - 10);
                        if (ImGui::ButtonEx(ICON_FA_CUBE)) {
                                std::filesystem::path p =
                                        e.GetComponent<PrefabComponent>().scene->scene_path;
                                p.replace_extension("scn");
                                SceneTabBar::ChangeScene(p);
                        }
                }
                if (has_script) {
                        ImGui::SameLine();
                        if (pos_x == -1.0f) {
                                pos_x = ImMax(0.0f, ImGui::GetContentRegionAvail().x);
                                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + pos_x - ImGui::CalcTextSize(ICON_FA_SCROLL ICON_FA_CUBE).x - 10);
                        } else {
                                ImGui::SetCursorPosX(ImGui::GetCursorPosX() - ImGui::CalcTextSize(ICON_FA_SCROLL " " ICON_FA_CUBE).x);
                        }
                        ImGui::Text(ICON_FA_SCROLL);
                }

                if (m_AllowReorder)
                        add_tree_drag_drop(e);

                for (int i = 1 - op; i > 0; i--) {
                        ImGui::TreePop();
                }
        }
}

}  // namespace Borek
