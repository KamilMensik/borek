// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/ParticleComponent.h"
#include "Include/Base/Colors.h"

namespace Borek {

ParticleComponent::ParticleComponent()
{
        this->size_variation = 0.0f;
        this->from_color = Colors::WHITE;
        this->to_color = Colors::WHITE;
        this->from_size = 16.0f;
        this->to_size = 16.0f;
        this->lifetime = 1.0f;
        this->emmits_per_second = 10;
        this->delta_cache = 0.0f;
        this->enabled = true;
}

}  // namespace Borek
