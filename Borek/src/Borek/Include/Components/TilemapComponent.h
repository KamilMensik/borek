// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Engine/Assets/TilemapAsset.h"

#include <cstdint>
#include <map>

namespace Borek {

struct TilemapComponent {
        std::map<std::pair<int16_t, int16_t>, uint16_t> items;
        std::map<std::pair<int16_t, int16_t>, uint32_t> existing_colliders;
        Asset<TilemapAsset> tilemap;
        uint16_t step_x = 32;
        uint16_t step_y = 32;

        // Without doing anything to physics world
        void
        Set(int16_t col, int16_t row, uint16_t item);

        // Without doing anything to physics world
        void
        Delete(int16_t col, int16_t row);

        // Updates physics world if necessary
        void
        Set(uint32_t entity_id, int16_t col, int16_t row, uint16_t item);

        // Updates physics world if necessary
        void
        Delete(uint32_t entity_id, int16_t col, int16_t row);
};

}  // namespace Borek
