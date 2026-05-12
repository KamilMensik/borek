// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Utils/StringHelpers.h"
#include <string>
#include <filesystem>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui/misc/cpp/imgui_stdlib.h>

#include <Include/Base/Node.h>
#include <Include/Debug/Log.h>
#include <Include/Base/Application.h>
#include <Include/Core.h>

#include "ExportPopup.h"
#include "Include/Base/Project.h"
#include "Misc/FileExplorer/FileExplorer.h"
#include "Misc/FontAwesome.h"

namespace fs = std::filesystem;

using Borek::HashP;
using Borek::Hash;

static void
copy_project_file(const fs::directory_entry& file, const fs::path& out)
{
        const fs::path& path = file.path();

        switch (HashP(path.extension())) {
        case Hash(".sst"):
        case Hash(".tex"):
        case Hash(".scn"):
        case Hash(".scnf"):
        case Hash(".anim"):
        case Hash(".snd"):
        case Hash(".scr"):
        case Hash(".mp3"):
        case Hash(".wav"):
        case Hash(".flac"):
        case Hash(".tmap"):
                fs::copy_file(path, out / path.filename(), fs::copy_options::overwrite_existing);
                break;
        default:
                break;
        }
}

static void
copy_project_folder(const fs::path& dir, const fs::path& out)
{
        fs::create_directories(out);
        for (auto& entry : fs::directory_iterator(dir)) {
                if (entry.is_directory() && entry.path().stem() != "build") {
                        copy_project_folder(entry.path(), out / entry.path().stem());
                } else if (entry.is_regular_file()) {
                        copy_project_file(entry, out);
                }
        }
}

namespace Borek {
namespace Popups {

bool
ExportPopup::Tick()
{
        static std::string game_name;

        Popup::Tick();
        bool is_open = true;
        ImGui::Begin("Export", &is_open);

        auto p = Project::InstanceM();


        ImGui::Text("game_name: ");
        ImGui::SameLine();
        ImGui::InputText("##gamename", &game_name);
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

        if (ImGui::Button("Export")) {
                fs::path ppath(Application::ProjectPath());
                fs::path out = ppath / "build";

                copy_project_folder(ppath, out);
                fs::copy_file("/usr/local/bin/borek", out / game_name,
                              fs::copy_options::overwrite_existing);
                fs::copy_file(ppath / "project.bproj", out / "project.bproj",
                              fs::copy_options::overwrite_existing);

                is_open = false;
        }

        ImGui::End();

        return !is_open; 
}

}  // namespace Popups
}  // namespace Borek

