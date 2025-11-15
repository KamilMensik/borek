// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"

namespace Borek {

class ZIndexAssigner {
public:
        static void
        Assign();

        static float
        GetTop() { return s_Current; };

private:
        static void
        AssignHelper(Entity e);

        static float s_Current;
};

}  // namespace Borek
