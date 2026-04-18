// Copyright 2024-2025 <kamilekmensik@gmail.com>

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
#include <Borek/Include/Components/PrefabComponent.h>
#include <Borek/Include/Engine/SceneSerializer.h>

#include "Scene.h"
#include "Popups/AddEntityPopup.h"
#include "Events/Events.h"
#include "Popups/AddScriptPopup.h"
#include "Misc/SceneTreeExplorer.h"
#include "Panels/PanelEvents.h"
#include "Commands/EntityCommands.h"
#include "Misc/FileExplorer/FileExplorer.h"
#include "Popups/ChangeNodeTypePopup.h"

namespace Borek
{
namespace Panels
{

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
        static Entity entity_for_prefab;

        if (!ImGui::Begin("Scene")) {
                ImGui::End();
                return;
        }

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

        SceneTreeExplorer().AllowReorder()
                .OnClick([this](Entity e) {
                        if (m_SelectedEntity == e)
                                return;

                        m_SelectedEntity = e;
                        SendOnSelectionChangeEvent();
                }).ContextMenu([](Entity e){
                        if (e.IsNil()) {
                                if (ImGui::MenuItem("Create Node")) {
                                        Application::OpenPopup(new Popups::AddEntityPopup(Application::GetScene()->GetTree().GetRootEntity()));
                                }
                                return;
                        }

                        if (ImGui::MenuItem("Add Child")) {
                                Application::OpenPopup(new Popups::AddEntityPopup(e));
                        }
                        if (ImGui::MenuItem("Change Node Type")) {
                                Application::OpenPopup(new Popups::ChangeNodeTypePopup(e));
                        }
                        if (ImGui::MenuItem("Remove Node")) {
                                Application::SendEvent<EditorCommandEvent>(NewUniq<RemoveEntityCommand>(e));
                        }
                        if (!e.HasComponent<PrefabComponent>()) {
                                if (ImGui::MenuItem("Save As Scene")) {
                                        entity_for_prefab = e;
                                        FileExplorer::Open("Save As Scene", FileExplorerType_SaveFile, Application::ProjectPath());
                                }
                        } else {
                                if (ImGui::MenuItem("Unprefab")) {
                                        e.RemoveComponent<PrefabComponent>();
                                }
                        }
                }).Search(m_Search)
                .Selected(m_SelectedEntity)
                .Draw();

        ImGui::End();

        if (FileExplorer::Begin("Save As Scene")) {
                auto tc = entity_for_prefab.Transform();
                entity_for_prefab.Transform() = TransformComponent();

                auto scn = Application::GetScene();
                Entity croot = scn->GetTree().GetRootEntity();
                scn->GetTree().SetRootEntity(entity_for_prefab);
                SceneAsset ass;
                ass.scene_path = FileExplorer::GetSelected();
                ass.scene_path += 'f';
                ass.Serialize(FileExplorer::GetSelected());
                SceneSerializer(scn).Serialize(ass.scene_path);
                scn->GetTree().SetRootEntity(croot);

                entity_for_prefab.AddComponent<PrefabComponent>();
                auto& prefab = entity_for_prefab.GetComponent<PrefabComponent>();
                prefab.scene = AssetManager::Get<SceneAsset>(FileExplorer::GetSelected());
                prefab.last_modified = std::filesystem::last_write_time(FileExplorer::GetSelected());

                entity_for_prefab.Transform() = tc;
        }
        FileExplorer::End();
}

void Scene::SendOnSelectionChangeEvent()
{
        Application::SendEvent<ScenePanelSelectedEvent>(m_SelectedEntity);
}

bool
Scene::OnChangeEntity(ChangeEntityEvent& e)
{
        m_SelectedEntity = e.GetEntity();
        return true;
}


} // namespace Panels
} // namespace Borek
