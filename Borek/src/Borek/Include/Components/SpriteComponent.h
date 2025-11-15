// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Colors.h"
#include "Include/Core.h"
#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/TexAsset.h"

namespace Borek {

enum SpriteComponentFlags {
        SpriteComponentFlags_Flipping = 1 | 2,
        SpriteComponentFlags_FlipX = 1,
        SpriteComponentFlags_FlipY = 2,
};

struct SpriteComponent {
        Asset<TexAsset> texture;
        Color color;
        BitFlags flags;

        SpriteComponent() : color(Colors::WHITE) {}
        SpriteComponent(Asset<TexAsset>& texture,
                        Color color = Colors::WHITE)
                : texture(texture), color(color) {}
};


}  // namespace Borek
