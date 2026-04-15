// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"

namespace Borek {

class EntityUninitializer {
public:
        static void
        UninitializeEntity(Entity e);

        static void
        UninitializeBegin(Entity e);

        static void
        UninitializeEnd(Entity e);

private:
        static TransformComponent s_GlobalTransform;

        static void
        UninitializeFZXBody(Entity e);

        static void
        UninitializeFZXArea(Entity e);

        static void
        UninitializeTilemap(Entity e);

        static void
        UninitializeSoundPlayer(Entity e);

        static void
        UninitializeRubyNode(Entity e);
};

}  // namespace Borek
