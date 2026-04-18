// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"

namespace Borek {

class EntityInitializer {
public:
        static void
        InitializeEntity(Entity e);

        static void
        InitializeBegin(Entity e);

        static void
        InitializeEnd(Entity e);

private:
        static void
        InitializeFZXBody(Entity e);

        static void
        InitializeFZXArea(Entity e);

        static void
        InitializeTilemap(Entity e);

        static void
        InitializeSoundPlayer(Entity e);

        static void
        InitializeRubyNode(Entity e);

        static void
        InitializeAnimatedSprite(Entity e);
};

}  // namespace Borek
