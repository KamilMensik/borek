// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>

namespace Borek {

enum AssetType {
        AssetType_Tex,
        AssetType_SpriteSheet,
        AssetType_Tilemap,
        AssetType_Script,
        AssetType_Sound,
        AssetType_Scene,
};

class IAsset {
public:
        virtual ~IAsset() = default;

        virtual void
        Serialize(const std::filesystem::path& path) = 0;

        virtual void
        Deserialize(const std::filesystem::path& path) = 0;

        virtual void
        Load() = 0;

        inline void
        LoadFrom(const std::filesystem::path& path)
        {
                Deserialize(path);
                Load();
        }

        virtual void
        Unload() = 0;

        virtual uint32_t
        GetType() const = 0;
};

}  // namespace Borek
