// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <algorithm>
#include <ranges>

#include <cstdint>
#include <format>
#include <imgui.h>
#include <ECS/DisabledComponent.h>

#include <Borek/Include/Base/Application.h>
#include <Borek/Include/Base/Project.h>
#include <Borek/Include/Engine/Assets/SceneAsset.h>
#include <Borek/Include/Engine/SceneSerializer.h>

#include "Misc/SceneTabBar.h"
#include "Misc/FontAwesome.h"
#include "Commands/CommandHandler.h"
#include "Misc/FileExplorer/FileExplorer.h"

namespace Borek {

void
SceneTabBar::Deinit()
{
        s_OpenedScenesByName.clear();
        s_OpenedScenes.clear();
}

void
SceneTabBar::ChangeScene(Ref<Scene> scene)
{
        auto it = s_OpenedScenesByName.find(scene->GetPath());
        uint32_t index;
        if (it != s_OpenedScenesByName.end()) {
                s_OpenedScenes[it->second].scene = scene;
                index = it->second;
        } else {
                index = s_OpenedScenes.size();
                s_OpenedScenes.emplace_back(scene, NewUniq<CommandHandler>(), true);
        }
        
        SwapScene(scene);
        s_OpenedScenesByName[scene->GetPath()] = index;
}

void
SceneTabBar::ChangeSceneEmpty()
{
        DisableScene(GetCurrentScene());
        Ref<Scene> new_scene = NewRef<Scene>();
        Application::SetScene(new_scene);

        for (int i = 0;; i++) {
                std::string anonymous_name = std::format("#{}", i);
                auto it = s_OpenedScenesByName.find(anonymous_name);
                if (it == s_OpenedScenesByName.end()) {
                        new_scene->SetPath(anonymous_name, false);
                        break;
                }
        }

        s_CurrentScene = s_OpenedScenes.size();
        s_ForceSelectScene = s_CurrentScene;
        s_OpenedScenes.emplace_back(new_scene, NewUniq<CommandHandler>(), true);
        s_OpenedScenesByName[new_scene->GetPath()] = s_CurrentScene;
        Project::InstanceM()->last_scene_opened_path = "";
}

void
SceneTabBar::ChangeScene(const std::filesystem::path& path, bool force)
{
        auto it = s_OpenedScenesByName.find(path);
        if (it != s_OpenedScenesByName.end()) {
                if (!force) {
                        SwapScene(s_OpenedScenes[it->second].scene);
                        return;
                }

                Ref<Scene> new_scene = NewRef<Scene>();
                Application::SetScene(new_scene);
                SceneAsset ass;
                ass.Deserialize(path);
                SceneSerializer(new_scene).Deserialize(ass.scene_path);

                s_OpenedScenes[it->second].scene = new_scene;
                s_CurrentScene = it->second;
                s_ForceSelectScene = s_CurrentScene;
                Project::InstanceM()->last_scene_opened_path = s_OpenedScenes[s_CurrentScene].scene->GetPath();
                return;
        }

        DisableScene(GetCurrentScene());
        Ref<Scene> new_scene = NewRef<Scene>();
        Application::SetScene(new_scene);

        if (!path.empty() && path.string()[0] != '#') {
                SceneAsset ass;
                ass.Deserialize(path);
                SceneSerializer(new_scene).Deserialize(ass.scene_path);
        } else {
                for (int i = 0;; i++) {
                        std::string anonymous_name = std::format("#{}", i);
                        auto it = s_OpenedScenesByName.find(anonymous_name);
                        if (it == s_OpenedScenesByName.end()) {
                                new_scene->SetPath(anonymous_name, false);
                                break;
                        }
                }
        }

        s_CurrentScene = s_OpenedScenes.size();
        s_ForceSelectScene = s_CurrentScene;
        s_OpenedScenes.emplace_back(new_scene, NewUniq<CommandHandler>(), true);
        s_OpenedScenesByName[new_scene->GetPath()] = s_CurrentScene;
        Project::InstanceM()->last_scene_opened_path =
                new_scene->GetPath().string()[0] == '#' ? "" : new_scene->GetPath();
}

void
SceneTabBar::RuntimeChangeScene(Ref<Scene> new_scene)
{
        s_OpenedScenes[s_CurrentScene].scene = new_scene;
}

void
SceneTabBar::RemoveScene(Ref<Scene> scene)
{
        const bool is_selected = s_CurrentScene == s_OpenedScenesByName.find(scene->GetPath())->second;
        auto it = std::ranges::remove(s_OpenedScenes, {scene});
        if (it.size() == 0)
                return;

        s_OpenedScenes.pop_back();
        s_OpenedScenesByName.erase(scene->GetPath());

        if (s_OpenedScenes.empty()) {
                scene = NewRef<Scene>();
                scene->Init();
                s_OpenedScenes.emplace_back(scene, NewUniq<CommandHandler>(), true);
                s_OpenedScenesByName["#0"] = 0;
                s_CurrentScene = 0;
                s_ForceSelectScene = 0;
                scene->SetPath("#0", false);
                Application::SetScene(scene);
                Project::InstanceM()->last_scene_opened_path = "";
        } else if (is_selected) {
                s_CurrentScene = s_OpenedScenes.size() - 1;
                s_ForceSelectScene = s_CurrentScene;
                EnableScene(s_OpenedScenes.back().scene);
                Application::SetScene(GetCurrentScene());
                Project::InstanceM()->last_scene_opened_path = s_OpenedScenes[s_CurrentScene].scene->GetPath();
        }
}

void
SceneTabBar::SaveScene()
{
        if (GetCurrentScene()->GetPath().string()[0] == '#') {
                FileExplorer::Open("Save Scene", FileExplorerType_SaveFile, Application::ProjectPath(),
                                   { ".scn" });
                return;
        }

        SceneAsset ass;
        ass.Deserialize(GetCurrentScene()->GetPath());
        s_OpenedScenes[s_CurrentScene].is_modified = false;
        SceneSerializer(Application::GetScene()).Serialize(ass.scene_path);
}

void
SceneTabBar::OnImguiRender()
{
        if (Application::IsPlaying())
                return;

        if (FileExplorer::Begin("Save Scene")) {
                SceneAsset ass;
                ass.scene_path = FileExplorer::GetSelected();
                ass.scene_path += 'f';
                ass.Serialize(FileExplorer::GetSelected());
                s_OpenedScenes[s_CurrentScene].is_modified = false;
                s_OpenedScenesByName.erase(GetCurrentScene()->GetPath());
                s_OpenedScenes[s_CurrentScene].scene->SetPath(FileExplorer::GetSelected());
                s_OpenedScenesByName[GetCurrentScene()->GetPath()] = s_CurrentScene;
                s_ForceSelectScene = s_CurrentScene;
                SceneSerializer(Application::GetScene()).Serialize(ass.scene_path);
        }
        FileExplorer::End();


        if (!ImGui::BeginTabBar("SceneTabBar", ImGuiTabBarFlags_Reorderable))
                return;

        std::vector<Ref<Scene>> to_remove;
        for (uint32_t i = 0; auto& [scene, cmd_handler, open, is_modified] : s_OpenedScenes) {
                if (!open) {
                        to_remove.emplace_back(scene);
                        i++;
                        continue;
                }

                if (cmd_handler->Changed()) is_modified = true;

                ImGuiTabItemFlags flags = ImGuiTabItemFlags_None;
                if (s_ForceSelectScene == i)
                        flags |= ImGuiTabItemFlags_SetSelected;
                if (is_modified)
                        flags |= ImGuiTabItemFlags_UnsavedDocument;

                std::string name = scene->GetPath().stem();
                std::string_view name_v = name;
                if (name[0] == '#') {
                        name = std::format("*unsaved*[{}]", name_v.substr(1));
                }
                if (ImGui::BeginTabItem(name.c_str(), &open, flags)) {
                        if (s_ForceSelectScene == UINT32_MAX && s_CurrentScene != i) {
                                DisableScene(GetCurrentScene());
                                s_CurrentScene = i;
                                EnableScene(scene);
                                Application::SetScene(scene);
                                Project::InstanceM()->last_scene_opened_path = scene->GetPath();
                        }
                        ImGui::EndTabItem();
                }

                i++;
        }

        s_ForceSelectScene = UINT32_MAX;

        if (ImGui::TabItemButton(ICON_FA_PLUS, ImGuiTabItemFlags_Trailing)) {
                ChangeSceneEmpty();
        }

        for (auto& scene : to_remove) {
                RemoveScene(scene);
        }

        ImGui::EndTabBar();
}

void
SceneTabBar::DisableScene(Ref<Scene> scene)
{
        if (!scene) return;

        ECS::World& w = *scene->m_World;
        SceneTree& st = scene->m_SceneTree;
        if (w.has_component<ECS::DisabledComponent>(st.GetRootEntity()))
                return;

        st.TraverseScene([&w](Entity e){
                w.add_component<ECS::DisabledComponent>(e);
        });
}

void
SceneTabBar::EnableScene(Ref<Scene> scene)
{
        if (!scene) return;

        ECS::World& w = *scene->m_World;
        SceneTree& st = scene->m_SceneTree;
        if (!w.has_component<ECS::DisabledComponent>(st.GetRootEntity()))
                return;

        st.TraverseScene([&w](Entity e){
                w.remove_component<ECS::DisabledComponent>(e);
        });
}

void
SceneTabBar::SwapScene(Ref<Scene> scene)
{
        if (GetCurrentScene() == scene)
                return;

        DisableScene(GetCurrentScene());
        uint32_t index = s_OpenedScenesByName[scene->GetPath()];
        s_CurrentScene = index;
        s_ForceSelectScene = index;
        EnableScene(scene);
        Application::SetScene(s_OpenedScenes[s_CurrentScene].scene);
        Project::InstanceM()->last_scene_opened_path = scene->GetPath();
}

CommandHandler&
SceneTabBar::GetCommandHandler()
{
        return *s_OpenedScenes[s_CurrentScene].cmd_handler;
}

const Ref<Scene>&
SceneTabBar::GetCurrentScene()
{
        static const Ref<Scene> nil = nullptr;
        if (s_CurrentScene == UINT32_MAX)
                return nil;

        return s_OpenedScenes[s_CurrentScene].scene;
}

std::vector<SceneTabBar::SceneData> SceneTabBar::s_OpenedScenes;
StringMap<uint32_t> SceneTabBar::s_OpenedScenesByName;
uint32_t SceneTabBar::s_CurrentScene = UINT32_MAX;
uint32_t SceneTabBar::s_ForceSelectScene = UINT32_MAX;

}  // namespace Borek
