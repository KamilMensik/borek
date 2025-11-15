// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Debug/Log.h"
#include <cstdint>
#include <fstream>

#include "Include/Engine/Assets/TilemapAsset.h"

namespace fs = std::filesystem;

namespace Borek {

TilemapAsset::~TilemapAsset()
{
}

void
TilemapAsset::Serialize(const fs::path &path)
{
        std::ofstream asset(path, std::ios::binary);

        if (sprite_sheet.IsValid()) {
                const uint32_t path_size = sprite_sheet.GetPath().size();
                asset.write(RCAST<const char*>(&path_size), sizeof(uint32_t))
                     .write(sprite_sheet.GetPath().c_str(), path_size);
        } else {
                const uint32_t path_size = 0;
                asset.write(RCAST<const char*>(&path_size), sizeof(uint32_t));
        }

        const uint32_t collider_size = tile_coliders.size();
        asset.write(RCAST<const char*>(&collider_size), sizeof(uint32_t))
             .write(RCAST<const char*>(tile_coliders.data()),
                    collider_size * sizeof(TileMapColliderBounds));
}

void
TilemapAsset::Deserialize(const fs::path &path)
{
        std::ifstream asset(path, std::ios::binary);
        Unload();

        uint32_t path_size;
        asset.read(RCAST<char*>(&path_size), sizeof(uint32_t));

        if (path_size > 0) {
                char* cpath = new char[path_size + 1];

                asset.read(cpath, path_size);
                cpath[path_size] = 0;

                sprite_sheet = AssetManager::Get<SpriteSheetAsset>(cpath);
                delete[] cpath;
        }

        uint32_t collider_size;
        asset.read(RCAST<char*>(&collider_size), sizeof(uint32_t));
        tile_coliders.resize(collider_size);
        asset.read(RCAST<char*>(tile_coliders.data()),
                   collider_size * sizeof(TileMapColliderBounds));
}

void
TilemapAsset::Load()
{
        if (sprite_sheet.IsValid() && tile_coliders.size() < sprite_sheet->GetCells()) {
                tile_coliders.resize(sprite_sheet->GetCells());
        }
}

void
TilemapAsset::Unload()
{
}

uint32_t
TilemapAsset::GetType() const
{
        return AssetType_Tilemap;
}

}  // namespace Borek
