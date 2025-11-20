// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Input.h"
namespace Borek {

/*
 * Abstract class that represents a tool used in editor viewport.
 */
class ITool {
public:
        virtual void
        OnMousePressed(MouseButton button) = 0;

        virtual void
        OnMouseReleased(MouseButton button) = 0;

        virtual bool
        Tick() = 0;

        virtual void
        Activate() = 0;

        virtual void
        Deactivate() = 0;
};

}  // namespace Borek
