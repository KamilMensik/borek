// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Base/Input.h"

namespace Borek {

glm::vec2 Input::GetAxis()
{
        using KC = KeyCode;

        return glm::vec2(
                IsKeyPressed(KC::A) * -1.0f + IsKeyPressed(KC::D) * 1.0f,
                IsKeyPressed(KC::S) * -1.0f + IsKeyPressed(KC::W) * 1.0f);
}

}
