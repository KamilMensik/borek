// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/Event.h"
#include "Include/Events/KeyEvents.h"
#include "Panels/PanelEvents.h"
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
        ~Tileset();

        void
        OnUpdate();

        void
        OnImGuiRender();

        bool
        OnChangeEntity(ChangeEntityEvent& e);

        bool
        OnMouseButton(MouseButtonEvent& ev);

        bool
        OnKey(KeyEvent& e);

private:
        Entity m_SelectedEntity;
        uint32_t m_SelectedIndex = 0;
        uint32_t m_SelectedToolIndex = UINT32_MAX;
        std::vector<ITilemapTool*> m_Tools;
        std::array<EVHandle, 3> m_EventHandles;
};

}  // namespace Panels
}  // namespace Borek
