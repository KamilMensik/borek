// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <filesystem>

#include "Include/Engine/AssetManager.h"
#include "Include/Engine/Utils/Settings.h"
#include "Include/Graphics/Texture.h"

namespace Borek {

void AssetManager::Reset()
{
        s_Assets.clear();
        s_LoadedAssets.clear();
        s_AssetPaths.clear();
}

Asset AssetManager::GetTexture(const std::string& path)
{
        if (auto res = s_LoadedAssets.find(path); res != s_LoadedAssets.end())
                return Asset{res->second};

        unsigned id = s_Assets.size();
        s_LoadedAssets[path] = id;
        if (path[0] == '/') {
                s_Assets.emplace_back(Graphics::Texture2D::Create(path));
        } else {
                s_Assets.emplace_back(Graphics::Texture2D::Create(std::filesystem::path(Utils::Settings::Instance().current_project_path)/path));
        }
        s_AssetPaths.emplace_back(path);

        return Asset{id};
}

std::unordered_map<std::string, unsigned> AssetManager::s_LoadedAssets;
std::vector<_Borek_AssetInternal> AssetManager::s_Assets;
std::vector<std::string> AssetManager::s_AssetPaths;

}  // namespace Borek
