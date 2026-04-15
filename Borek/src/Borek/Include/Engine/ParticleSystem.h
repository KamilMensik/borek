// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <vector>

#include <glm/ext/vector_float2.hpp>

#include "Include/Engine/Particle.h"

namespace Borek {

class ParticleSystem {
public:
        static void
        OnUpdate(float delta);

        static void
        Render();

        static void
        Emit(const ParticleParams& params, const glm::vec2& position,
             float zindex);

private:
        static std::vector<Particle> s_Pool;
};

}  // namespace Borek
