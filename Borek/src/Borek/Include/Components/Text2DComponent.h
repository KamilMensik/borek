// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <glm/vec4.hpp>

#include "Include/Engine/Assets/Asset.h"
#include "Include/Engine/Assets/FontAsset.h"
#include "Include/Base/Colors.h"
#include "Include/Base/Symbol.h"

namespace Borek {

struct Text2DComponent {
        Symbol value;
        Color color;
        float size;
        Asset<FontAsset> font;

        Text2DComponent() = default;
};

}  // namespace Borek
