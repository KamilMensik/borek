// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Utils/Settings.h"
#include "Include/Scripting/Ruby/RubyScript.h"
#include <string>
#include <unordered_map>
#include <vector>

#include "Include/Graphics/Texture.h"
#include "Include/Engine/Assets/Asset.h"

namespace Borek {

using _Borek_AssetInternal =
std::variant<Ref<Graphics::Texture2D>, RubyScript>;

class AssetManager {
friend class SceneSerializer;
public:
        static void
        Reset();

        inline static Ref<Graphics::Texture2D>
        GetTexture(unsigned id)
        {
                return std::get<Ref<Graphics::Texture2D>>(s_Assets[id]);
        }

        static Asset
        GetTexture(const std::string& path);

        static Asset
        GetAsset(const std::string& path);

        static Ref<Graphics::Texture2D>
        GetTextureRaw(const std::filesystem::path& path,
                      const std::string& extension = "tex");

        static inline const std::string&
        GetPath(unsigned id)
        {
                return s_AssetPaths[id];
        }

        static Asset::Type
        GetAssetType(unsigned id)
        {
                switch (s_Assets[id].index()) {
                case 0:
                        return Asset::Type::kImage;
                case 1:
                        return Asset::Type::kScript;
                }

                BOREK_ASSERT(false, "????????");
                return Asset::Type::kImage;
        }

        static void
        RefreshTexture(const std::filesystem::path& path,
                       const std::filesystem::path& asset_path);

        static void
        RefreshScript(const std::filesystem::path& path,
                      const std::filesystem::path& brass_path);

        static void
        AssetifyWorkspace(const std::string& path, bool force = false);

        static void
        AssetifyImage(const std::filesystem::path& path,
                      const std::filesystem::path& asset_path);

private:
        static std::unordered_map<std::string, unsigned> s_LoadedAssets;
        static std::vector<_Borek_AssetInternal> s_Assets;
        static std::vector<std::string> s_AssetPaths;
};

}  // namespace Borek
