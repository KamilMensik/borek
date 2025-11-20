// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <array>

#include "Include/Events/Event.h"
#include "Panels/PanelEvents.h"
#include <Include/Base/Entity.h>

namespace Borek {
namespace Panels {

class GizmoPanel {
public:
        enum class Mode {
                kNothing = -1,
                kTranslate = 0,
                kRotate = 1,
                kScale = 2,
        };
public:
        GizmoPanel();
        ~GizmoPanel();
        void OnChangeEntity(ChangeEntityEvent& ev);
        void DrawGizmo();
        void SetMode(Mode m);

private:
        Entity m_Entity;
        int m_CurrentMode = -1;
        std::array<EVHandle, 1> m_EventHandles;
};

}  // namespace Panels
}  // namespace Borek
