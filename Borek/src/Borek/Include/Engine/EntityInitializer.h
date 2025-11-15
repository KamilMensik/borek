// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"

namespace Borek {

class EntityInitializer {
public:
        static void
        InitializeBegin(Entity e);

        static void
        InitializeEnd(Entity e);

private:
        static TransformComponent s_GlobalTransform;

        static void
        InitializeFZXBody(Entity e);

        static void
        InitializeTilemap(Entity e);

        static void
        InitializeSoundPlayer(Entity e);
};

}  // namespace Borek
