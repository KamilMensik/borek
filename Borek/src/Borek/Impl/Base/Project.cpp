// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Application.h"
#include <filesystem>
#include <fstream>
#include <string>

#include <yaml-cpp/yaml.h>

#include "Include/Base/Project.h"

namespace Borek {

void
Project::Serialize(const std::string& path)
{
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "last_scene_opened_path" << YAML::Value << s_Instance.last_scene_opened_path;
        out << YAML::Key << "start_scene_path" << YAML::Value << s_Instance.start_scene_path;
        out << YAML::EndMap;

        std::ofstream fout(path);
        fout << out.c_str();
}

void
Project::Deserialize(const std::string& path)
{
        if (std::filesystem::exists(path)) {
                YAML::Node data = YAML::LoadFile(path);
                s_Instance.last_scene_opened_path = data["last_scene_opened_path"].as<std::string>();
                s_Instance.start_scene_path = data["start_scene_path"].as<std::string>();
        }
}

void
Project::OnChange()
{
        Serialize(std::filesystem::path(Application::ProjectPath()) / "project.bproj");
}

Project Project::s_Instance;

}  // namespace Borek
