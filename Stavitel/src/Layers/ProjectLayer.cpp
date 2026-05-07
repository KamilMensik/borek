// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <filesystem>
#include <fstream>

#include <yaml-cpp/emittermanip.h>
#include <yaml-cpp/yaml.h>
#include <imgui.h>
#include "Borek/Include/Base/Application.h"
#include "Borek/Include/Engine/Utils/FileUtils.h"

#include "Layers/ProjectLayer.h"
#include "Events/ProjectEvents.h"
#include "Misc/FileExplorer/FileExplorer.h"

namespace Borek {

static void
generate_project(const std::filesystem::path& path)
{
        std::filesystem::create_directories(path);
        YAML::Emitter out;

        out << YAML::BeginMap;
        out << YAML::Key << "last_scene_opened_path" << YAML::Value << "";
        out << YAML::Key << "start_scene_path" << YAML::Value << "";
        out << YAML::EndMap;

        std::ofstream fout(path / "project.bproj");
        fout << out.c_str();
}

void
ProjectLayer::OnAttach()
{
}

void
ProjectLayer::OnDetach()
{
}

void
ProjectLayer::OnUpdate(float delta)
{
}

void 
ProjectLayer::OnImGuiRender()
{
        static ImGuiDockNodeFlags_ dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
                window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        if (ImGui::Begin("Project", nullptr, window_flags)) {
                ImGui::PopStyleVar();
                ImGui::PopStyleVar(2);
                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
                ImGui::Text("BOREK STAVITEL");
                if (ImGui::Button("Create a new project!")) {
                        FileExplorer::Open("Create Project",
                                           FileExplorerType_OpenFile,
                                           Utils::UserDataPath(),
                                           {},
                                           FileExplorerFlags_OnlyDirectories);
                }
                if (ImGui::Button("Open an existing project!")) {
                        FileExplorer::Open("Open Project",
                                           FileExplorerType_OpenFile,
                                           Utils::UserDataPath(),
                                           { ".bproj" });
                }
                ImGui::PopFont();

                if (FileExplorer::Begin("Create Project")) {
                        std::filesystem::path path = FileExplorer::GetSelected();
                        path.replace_extension("");
                        generate_project(path);
                        Application::SendEvent<SwitchProjectEvent>(path);
                }
                FileExplorer::End();
                if (FileExplorer::Begin("Open Project")) {
                        std::filesystem::path path = FileExplorer::GetSelected();
                        Application::SendEvent<SwitchProjectEvent>(path.parent_path());
                }
                FileExplorer::End();
        }
        ImGui::End();
}

}  // namespace Borek
