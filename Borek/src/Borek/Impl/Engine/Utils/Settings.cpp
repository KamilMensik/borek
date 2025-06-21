// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Utils/Settings.h"
#include "Include/Engine/Utils/FileUtils.h"
#include "yaml-cpp/yaml.h"
#include <filesystem>
#include <fstream>

namespace Borek {
namespace Utils {

Settings::Settings()
{
        std::filesystem::create_directories(Utils::UserDataPath());
        Deserialize();
}

void Settings::OnChange()
{
        Serialize();
}

void Settings::Serialize()
{
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "current_project_path" << YAML::Value << current_project_path;
        out << YAML::Key << "last_scene_opened_path" << YAML::Value << last_scene_opened_path;

        std::ofstream fout(Utils::UserDataPath().append("/.engine.yaml"));
        fout << out.c_str();
}

void Settings::Deserialize()
{
        std::string settings_path = Utils::UserDataPath().append("/.engine.yaml");
        std::ifstream f(settings_path);
        if (f.good()) {
                YAML::Node data = YAML::LoadFile(settings_path);
                this->current_project_path = data["current_project_path"].as<std::string>();
                this->last_scene_opened_path = data["last_scene_opened_path"].as<std::string>();
        } else {
                this->current_project_path = Utils::OpenFolderDialog(UserHomePath().c_str());
                Serialize();
        }
}

Settings Settings::s_Instance;

}  // namespace Utils
}  // namespace Borek
