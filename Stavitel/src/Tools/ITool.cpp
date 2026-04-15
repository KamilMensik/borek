// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Tools/ITool.h"
#include "glm/common.hpp"

namespace Borek {

bool
ITool::IsInsideViewport() const
{
        const glm::vec2 rel_pos = glm::abs(Input::GetMousePosRelative());
        return rel_pos.x <= 1 && rel_pos.y <= 1;
}

}  // namespace Borek
