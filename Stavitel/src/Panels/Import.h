// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Core.h"
#include "Include/Engine/Assets/IAsset.h"
#include <filesystem>
#include <string>

namespace Borek {
namespace Panels {

class Import {
public:
        Import();
        void OnImguiRender();
        void SetSelectedAsset(const std::string& asset_path);

private:
        std::filesystem::path m_SelectedAssetPath;
        Uniq<IAsset> m_SelectedAsset;
        
        void
        TextureImport();

        void
        SpriteSheetImport();

        void
        ScriptImport();

        void
        SoundImport();
};

}  // namespace Panels
}  // namespace Borek
