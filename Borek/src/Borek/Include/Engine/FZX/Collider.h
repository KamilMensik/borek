// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

/*
*       Collision boxes for Borek.
*       Each collision shape is internally represented as a rectangle.
*/

#include "Include/Platform/SIMD.h"

namespace Borek {
namespace FZX {

// Collider component.
struct Collider {
        // [x1, y1, x2, y2]
        SimdVec4f box;
};

}
}  // namespace Borek
