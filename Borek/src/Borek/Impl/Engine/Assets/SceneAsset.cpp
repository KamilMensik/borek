// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <filesystem>
#include <fstream>

#include "Include/Engine/Assets/SceneAsset.h"
#include "Include/Engine/Assets/IAsset.h"
#include "Include/Engine/Utils/PathHelpers.h"
#include <iostream>

namespace Borek {

namespace fs = std::filesystem;

void
SceneAsset::Serialize(const std::filesystem::path &path)
{
        std::ofstream stream(path);
        stream << Utils::Path::ToRelative(scene_path).string();

        if (fs::exists(scene_path))
                return;

        std::ofstream clear_scene(scene_path);
        clear_scene
                << "Scene: Player\n"
                << "Root:\n"
                << "  Name: Player\n"
                << "  Type: Node\n"
                << "  Components:\n"
                << "    Transform:\n"
                << "      Position: [0, 0]\n"
                << "      Scale: [1, 1]\n"
                << "      Rotation: 0\n";
}

void
SceneAsset::Deserialize(const std::filesystem::path &path)
{
        std::ifstream stream(path);
        stream >> scene_path;
        scene_path = Utils::Path::FromRelative(scene_path);
}

void
SceneAsset::Load()
{
}

void
SceneAsset::Unload()
{
}

uint32_t
SceneAsset::GetType() const
{
        return AssetType_Scene;
}

}  // namespace Borek
