// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

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
        void ChangeEntity(Entity e);
        void DrawGizmo();
        void SetMode(Mode m);

private:
        Entity m_Entity;
        int m_CurrentMode;
};

}  // namespace Panels
}  // namespace Borek
