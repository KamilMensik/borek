// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

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
        static void Reset();
        inline static Ref<Graphics::Texture2D> GetTexture(unsigned id)
        {
                return std::get<Ref<Graphics::Texture2D>>(s_Assets[id]);
        }

        static Asset GetTexture(const std::string& path);
        static inline const std::string& GetPath(unsigned id)
        {
                return s_AssetPaths[id];
        }

        static void RefreshTexture(const std::string& path);
        static void RefreshScript(const std::string& path);

        static void AssetifyWorkspace(const std::string& path, bool force = false);

private:
        static std::unordered_map<std::string, unsigned> s_LoadedAssets;
        static std::vector<_Borek_AssetInternal> s_Assets;
        static std::vector<std::string> s_AssetPaths;
};

}  // namespace Borek
