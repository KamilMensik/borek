// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

#include "Include/Engine/Assets/IAsset.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Engine/Assets/Asset.h"

namespace Borek {

struct TileMapColliderBounds {
        uint8_t x, y, width, height;

        inline bool
        Exists() { return width != 0; }

        inline void
        Reset() { x = 0; y = 0; width = 0; height = 0; }
};

struct TilemapAsset : public IAsset {
        ~TilemapAsset() override;

        void
        Serialize(const std::filesystem::path &path) override;

        void
        Deserialize(const std::filesystem::path &path) override;

        void
        Load() override;

        void
        Unload() override;

        uint32_t
        GetType() const override;

        std::vector<TileMapColliderBounds> tile_coliders;
        Asset<SpriteSheetAsset> sprite_sheet;
        uint32_t flags;
};


}  // namespace Borek
