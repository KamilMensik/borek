// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <glm/ext/vector_float2.hpp>

#include "Include/Base/Colors.h"

namespace Borek {

struct ParticleParams {
        glm::vec2 velocity, velocity_variation;
        Color from_color, to_color;
        float from_size, to_size, size_variation;
        float lifetime;
};

struct Particle {
        glm::vec2 position;
        glm::vec2 velocity;
        Color from_color, to_color;
        float rotation;
        float from_size, to_size;
        float lifetime;
        float lifetime_remaining;
        float zindex;
};

}  // namespace Borek
