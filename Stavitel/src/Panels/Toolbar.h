// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/MouseEvents.h"
#include <array>
#include <vector>

#include <Borek/Include/Core.h>
#include <Borek/Include/Events/Event.h>

#include "Panels/PanelEvents.h"
#include "Tools/IToolbarTool.h"

namespace Borek {
namespace Panels {

class Toolbar {
public:
        Toolbar();
        ~Toolbar();

        void
        Tick();

        void
        OnImguiRender();

        void
        OnChangeEntity(ChangeEntityEvent& ev);

        void
        OnMouseButton(MouseButtonEvent& ev);

private:
        std::vector<Uniq<IToolbarTool>> m_Tools;
        std::array<EVHandle, 2> m_EventHandles;
};

}  // namespace Panels
}  // namespace Borek
