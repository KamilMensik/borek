// Copyright 2024-2025 <kamilekmensik@gmail.com>

#include "Include/Engine/FZX/Body.h"
#include "Include/Engine/FZX/Collider.h"
#include "Include/Platform/SIMD.h"

namespace Borek {
namespace FZX {

bool
Body::CollidesWith(const Body& other) const
{
        return true;
}

void
Body::Update(const glm::vec2& pos, const glm::vec2& size)
{
        const SimdVec4f pos_v(pos.x, pos.y, pos.x, pos.y);
        const SimdVec4f size_v(0, 0, size.x, size.y);

        collider.box = simd_add4f(pos_v, size_v);
}

} // namespace FZX
}  // namespace Borek
