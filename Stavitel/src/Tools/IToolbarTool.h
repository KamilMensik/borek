// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"

#include "Tools/ITool.h"

namespace Borek {

class IToolbarTool : public ITool {
public:
        virtual void
        SetEntity(Entity e) = 0;
};

}  // namespace Borek
