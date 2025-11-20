// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <array>
#include <filesystem>
#include <string>

#include "Events/Events.h"
#include "Include/Core.h"
#include "Include/Engine/Assets/IAsset.h"

namespace Borek {
namespace Panels {

class Import {
public:
        Import();
        ~Import();
        void OnImguiRender();
        void SetSelectedAsset(const std::string& asset_path);

private:
        std::filesystem::path m_SelectedAssetPath;
        Uniq<IAsset> m_SelectedAsset;
        std::array<EVHandle, 1> m_EvHandles;
        
        bool
        OnAssetPanelSelected(AssetPanelSelectedEvent& e);

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
