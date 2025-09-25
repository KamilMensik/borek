// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Graphics/Texture.h"
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
        std::filesystem::path m_SelectedAsset;
        Ref<Graphics::Texture2D> m_AssetTexture;
};

}  // namespace Panels
}  // namespace Borek
