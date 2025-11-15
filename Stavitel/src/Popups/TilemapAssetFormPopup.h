// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <filesystem>
#include <string>

#include "Include/Base/Popup.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/SpriteSheetAsset.h"
#include "Include/Engine/Assets/TilemapAsset.h"

namespace Borek {
namespace Popups {

class TilemapAssetFormPopup : public Popup {
public:
        TilemapAssetFormPopup(const std::filesystem::path& path);
        TilemapAssetFormPopup(const std::filesystem::path& path,
                              const std::filesystem::path& tmap_path);

        bool Tick() override;

private:
        Asset<SpriteSheetAsset> m_SpriteSheet;
        std::filesystem::path m_Path;
        std::string m_SpriteSheetPath;
        std::string m_AssetName = "tilemap.tmap";
        Uniq<TilemapAsset> m_Tilemap;
        bool m_NewSpriteSheet;
};

}  // namespace Popups
}  // namespace Borek
