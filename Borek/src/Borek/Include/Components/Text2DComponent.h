// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <glm/vec4.hpp>

#include "Include/Core.h"
#include "Include/Base/String.h"

namespace Borek {

struct Text2DComponent {
        Ref<class Font> font;
        String value;
        glm::vec4 color;
        float size;

        Text2DComponent() = default;
};

}  // namespace Borek
