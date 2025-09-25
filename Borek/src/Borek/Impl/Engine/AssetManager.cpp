// Copyright 2024-2025 <kamilekmensik@gmail.com>


#include "Include/Debug/Log.h"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ostream>

#include <stb/image.h>

#include "Include/Engine/Assets/AssetManager.h"
#include "Include/Engine/Utils/Settings.h"
#include "Include/Graphics/Texture.h"
#include "Include/Scripting/Ruby/Compiler.h"
#include "Include/Base/Application.h"
#include "Include/Core.h"

namespace Borek {

namespace fs = std::filesystem;

static constexpr uint32_t
Hash(const std::string& str)
{
        uint32_t res = 5381;

        for (auto c : str) {
                res = (res << 5) + res + c;
        }

        return res;
}

static void
AssetifyScript(const fs::path& file, const fs::path& brass_file)
{
        AssetManager::RefreshScript(file, brass_file);
}

void AssetManager::AssetifyImage(const fs::path& path, const fs::path& asset_path)
{
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* img = stbi_load(path.c_str(), &width, &height,
                                 &channels, STBI_default);

        BOREK_ENGINE_ASSERT(img, "Image from path {} could not be loaded", path.c_str());

        std::ofstream asset(asset_path, std::ios::binary);

        asset.write(RCAST<char*>(&width), sizeof(int))
             .write(RCAST<char*>(&height), sizeof(int))
             .write(RCAST<char*>(&channels), sizeof(int))
             .write(RCAST<char*>(img), width * height * channels);

        stbi_image_free(img);

        AssetManager::RefreshTexture(path, asset_path);
}

static void
AssetifyFile(const fs::path& file, bool force)
{
        switch (Hash(file.extension())) {
        case Hash(".tex"):
        case Hash(".scr"):
        case Hash(".fnt"):
                return;
        }

        fs::path asset_path = file;

        switch (Hash(file.extension())) {
        case Hash(".png"):
        case Hash(".jpg"):
        case Hash(".jpeg"):
                asset_path = asset_path.replace_extension("tex");
                if (!force && fs::exists(asset_path) &&
                    (fs::last_write_time(file) <= fs::last_write_time(asset_path)))
                        return;
                AssetManager::AssetifyImage(file, asset_path);
                BOREK_ENGINE_INFO("|AssetManager| - Compiled {}", file.string());
                break;
        case Hash(".rb"):
                asset_path = asset_path.replace_extension("scr");
                if (!force && fs::exists(asset_path) &&
                    (fs::last_write_time(file) <= fs::last_write_time(asset_path)))
                        return;
                AssetifyScript(file, asset_path);
                BOREK_ENGINE_INFO("|AssetManager| - Compiled {}", file.string());
                break;
        default:
                break;
        }
}

void
AssetifyFolder(const fs::path& dir, bool force)
{
        for (auto& file : fs::directory_iterator(dir)) {
                if (file.is_directory())
                        AssetifyFolder(file, force);
                else
                        AssetifyFile(file, force);
        };
}

void AssetManager::Reset()
{
        s_Assets.clear();
        s_LoadedAssets.clear();
        s_AssetPaths.clear();
}

Asset
AssetManager::GetTexture(const std::string& path)
{
        fs::path real_path = fs::path(Utils::Settings::Instance().current_project_path) / path;
        if (auto res = s_LoadedAssets.find(path); res != s_LoadedAssets.end())
                return Asset{res->second};

        fs::path asset_path = real_path.replace_extension("tex");

        AssetifyFile(real_path, false);

        unsigned id = s_Assets.size();
        s_LoadedAssets[asset_path] = id;
        s_Assets.emplace_back(Graphics::Texture2D::Create(asset_path));
        s_AssetPaths.emplace_back(asset_path);

        return Asset{id};
}

Asset
AssetManager::GetAsset(const std::string& path)
{
        if (auto res = s_LoadedAssets.find(path); res != s_LoadedAssets.end()) {
                return Asset(res->second);
        }

        return Asset();
}

Ref<Graphics::Texture2D>
AssetManager::GetTextureRaw(const fs::path &path, const std::string& extension)
{
        if (path.extension() == ".tex")
                return Graphics::Texture2D::Create(path);

        fs::path asset_path = path;
        asset_path.replace_extension(extension);

        AssetifyFile(path, false);
        return Graphics::Texture2D::Create(asset_path);
}

void AssetManager::RefreshTexture(const fs::path& path, const fs::path& asset_path)
{
        fs::path relative = fs::relative(asset_path, Utils::Settings::Instance().current_project_path);
        if (auto res = s_LoadedAssets.find(relative); res != s_LoadedAssets.end()) {
                s_Assets[res->second] = Graphics::Texture2D::Create(asset_path);
        }
}

void AssetManager::RefreshScript(const fs::path& path, const fs::path& brass_path)
{
        fs::path relative = fs::relative(brass_path, Utils::Settings::Instance().current_project_path);
        if (auto res = s_LoadedAssets.find(relative); res != s_LoadedAssets.end()) {
                Application::GetRubyEngine().GetCompiler()
                           .Compile(path, brass_path);
                s_Assets[res->second] = RubyScript(brass_path);
        } else {
                s_LoadedAssets[relative] = s_Assets.size();
                s_Assets.emplace_back(RubyScript(brass_path));
                s_AssetPaths.emplace_back(brass_path);
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
