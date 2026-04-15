// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Particle.h"

namespace Borek {

struct ParticleComponent : public ParticleParams {
        ParticleComponent();

        float delta_cache;
        uint32_t emmits_per_second;
        bool enabled;
};

}  // namespace Borek
