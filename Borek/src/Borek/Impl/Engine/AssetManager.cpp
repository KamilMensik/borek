// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ostream>
#include <print>

#include "Include/Engine/Assets/AssetManager.h"
#include "Include/Engine/Utils/Settings.h"
#include "Include/Graphics/Texture.h"

namespace Borek {

namespace fs = std::filesystem;

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
                s_Assets.emplace_back(Graphics::Texture2D::Create(fs::path(Utils::Settings::Instance().current_project_path)/path));
        }
        s_AssetPaths.emplace_back(path);

        return Asset{id};
}

static constexpr uint32_t Hash(const std::string& str)
{
        uint32_t res = 5381;

        for (auto c : str) {
                res = (res << 5) + res + c;
        }

        return res;
}

void AssetifyImage(const fs::path& file, const fs::path& brass_file)
{
        std::ofstream asset(brass_file, std::ios::out);
        asset << Asset::Type::kImage << file;

        AssetManager::RefreshTexture(file);
}

void AssetifyScript(const fs::path& file, const fs::path& brass_file)
{
        std::ofstream asset(brass_file, std::ios::out);
        asset << Asset::Type::kScript << file;

        AssetManager::RefreshScript(file);
}

void AssetifyFile(const fs::path& file, bool force)
{
        if (file.extension() == ".brass")
                return;

        fs::path brass_file = file.string().append(".brass");
        if (!force && fs::exists(brass_file) &&
            (fs::last_write_time(file) <= fs::last_write_time(brass_file)))
                return;

        switch (Hash(file.extension())) {
        case Hash(".png"):
        case Hash(".jpg"):
        case Hash(".jpeg"):
                AssetifyImage(file, brass_file);
                break;
        case Hash(".rb"):
                AssetifyScript(file, brass_file);
                break;
        default:
                break;
        }
}

void AssetifyFolder(const fs::path& dir, bool force)
{
        for (auto& file : fs::directory_iterator(dir)) {
                if (file.is_directory())
                        AssetifyFolder(file, force);
                else
                        AssetifyFile(file, force);
        };
}
void AssetManager::RefreshTexture(const std::string& path)
{
        if (auto res = s_LoadedAssets.find(path); res != s_LoadedAssets.end()) {
                s_Assets[res->second] = Graphics::Texture2D::Create(path);
        }
}

void AssetManager::RefreshScript(const std::string& path)
{
        if (auto res = s_LoadedAssets.find(path); res != s_LoadedAssets.end()) {
                s_Assets[res->second] = RubyScript(path);
        } else {
                s_LoadedAssets[path] = s_Assets.size();
                s_Assets.emplace_back(RubyScript(path));
                s_AssetPaths.emplace_back(path);
        }
}

void AssetManager::AssetifyWorkspace(const std::string &path, bool force)
{
        AssetifyFolder(path, force);
}

std::unordered_map<std::string, unsigned> AssetManager::s_LoadedAssets;
std::vector<_Borek_AssetInternal> AssetManager::s_Assets;
std::vector<std::string> AssetManager::s_AssetPaths;

}  // namespace Borek
