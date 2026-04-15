// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <utility>

namespace Borek {

struct EditorSettings {
        static bool show_collision_shapes;
        static std::pair<uint32_t, uint32_t> grid_snap;
};

}  // namespace Borek
