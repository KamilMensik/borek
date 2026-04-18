// Copyright 2024-2026 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <unordered_map>

#include "Include/Components/TransformComponent.h"
#include "Include/Base/Entity.h"

namespace Borek {

class TransformCache {
public:
        static void
        Reset();

        static const TransformComponent&
        GetTransform(Entity e);

        static void
        Invalidate(Entity e);

private:
        static std::unordered_map<uint32_t, std::pair<bool, TransformComponent>> s_Map;
};

}  // namespace Borek
