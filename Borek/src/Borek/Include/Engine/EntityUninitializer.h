// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"

namespace Borek {

class EntityUninitializer {
public:
        static void
        UninitializeEntity(Entity e);

        static void
        Uninitialize(Entity e);

private:
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
