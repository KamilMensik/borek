// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Misc/FileExplorer/FileExplorer.h"
#include "Misc/FontAwesome.h"
#include <cstring>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <Include/Base/Node.h>
#include <Include/Debug/Log.h>
#include <Include/Base/Application.h>
#include <Include/Core.h>

#include "ProjectSettingsPopup.h"
#include "Include/Base/Project.h"

namespace Borek {
namespace Popups {

bool
ProjectSettingsPopup::Tick()
{
        Popup::Tick();
        bool is_open = true;
        ImGui::Begin("Project Settings", &is_open);

        auto p = Project::InstanceM();

        ImGui::Text("start_scene: %s", p->start_scene_path.c_str());
        ImGui::SameLine();
        if (ImGui::Button(ICON_FA_FOLDER)) {
                FileExplorer::Open("Select Start Scene",
                                   FileExplorerType_OpenFile,
                                   Application::ProjectPath(),
                                   { ".scn" });
        }

        if (FileExplorer::Begin("Select Start Scene")) {
                p->start_scene_path = FileExplorer::GetSelected();
        }
        FileExplorer::End();

        if (ImGui::Button("End")) {
                is_open = false;
        }

        ImGui::End();

        return !is_open; 
}

}  // namespace Popups
}  // namespace Borek

