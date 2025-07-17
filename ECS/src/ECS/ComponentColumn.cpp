// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "ComponentColumn.h"

namespace ECS {

void* ComponentColumn::operator[] (uint32_t index)
{
        return (uint8_t*)data + (element_size * index);
}

}  // namespace ECS
