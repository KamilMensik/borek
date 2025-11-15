// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Components/TilemapComponent.h"

namespace Borek {

void
TilemapComponent::Set(int16_t col, int16_t row, uint16_t item)
{
        items[{row, col}] = item;
}

void
TilemapComponent::Delete(int16_t col, int16_t row)
{
        items.erase({row, col});
}

}  // namespace Borek
