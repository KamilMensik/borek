// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>

#include "Include/Base/UUID.h"

namespace Borek {

struct IDComponent {
        UUID id;
        uint32_t ecs_id;

        IDComponent() : id() {}
        IDComponent(UUID id) : id(id) {}
        operator uint64_t() { return id; }
        operator UUID() { return id; }
};

}  // namespace Borek
