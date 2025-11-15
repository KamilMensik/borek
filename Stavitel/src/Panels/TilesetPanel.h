// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <cstdint>
#include <vector>

#include "Include/Base/Entity.h"
#include "Include/Events/MouseEvents.h"
#include "Tools/Tilemap/ITilemapTool.h"

namespace Borek {
namespace Panels {

class Tileset {
public:
        Tileset();

        void
        OnUpdate();

        void
        OnImGuiRender();

        void
        SetEntity(Entity e);

        bool
        OnMouseButtonPressed(MouseButtonPressedEvent& ev);

        bool
        OnMouseButtonReleased(MouseButtonReleasedEvent& ev);

private:
        Entity m_SelectedEntity;
        uint32_t m_SelectedIndex = 0;
        uint32_t m_SelectedToolIndex = UINT32_MAX;
        std::vector<ITilemapTool*> m_Tools;
};

}  // namespace Panels
}  // namespace Borek
