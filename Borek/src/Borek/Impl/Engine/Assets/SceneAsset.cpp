// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/Assets/SceneAsset.h"
#include "Include/Debug/Log.h"
#include "Include/Engine/Assets/IAsset.h"
#include "Include/Engine/Utils/PathHelpers.h"
#include <fstream>

namespace Borek {


void
SceneAsset::Serialize(const std::filesystem::path &path)
{
        std::ofstream stream(path);
        stream << Utils::Path::ToRelative(scene_path);
}

void
SceneAsset::Deserialize(const std::filesystem::path &path)
{
        std::ifstream stream(path);
        stream >> scene_path;
        scene_path = Utils::Path::FromRelative(scene_path);
        BOREK_ENGINE_INFO("Scene path: {}", scene_path);
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
