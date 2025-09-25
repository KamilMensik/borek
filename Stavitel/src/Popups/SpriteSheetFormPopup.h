// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Popup.h"
#include "Include/Graphics/Texture.h"
#include "Include/Objects/SpriteSheet.h"
#include <filesystem>
#include <string>

namespace Borek {
namespace Popups {

class SpriteSheetFormPopup : public Popup {
public:
        SpriteSheetFormPopup(const std::filesystem::path& path);
        SpriteSheetFormPopup(const std::filesystem::path& path,
                             Ref<SpriteSheet> spritesheet);

        bool Tick() override;

private:
        Ref<SpriteSheet> m_SpriteSheet;
        Ref<Graphics::Texture2D> m_CurrentTexture;
        std::filesystem::path m_Path;
        std::string m_AssetName = "SpriteSheet.sst";
        bool m_NewSpriteSheet;
};

}  // namespace Popups
}  // namespace Borek
