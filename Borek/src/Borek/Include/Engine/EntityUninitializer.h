// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"

namespace Borek {

class EntityUninitializer {
public:
        static void
        UninitializeBegin(Entity e);

        static void
        UninitializeEnd(Entity e);

private:
        static TransformComponent s_GlobalTransform;

        static void
        UninitializeFZXBody(Entity e);

        static void
        UninitializeTilemap(Entity e);

        static void
        UninitializeSoundPlayer(Entity e);
};

}  // namespace Borek
