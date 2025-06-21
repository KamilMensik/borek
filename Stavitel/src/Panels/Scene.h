// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include <Borek/Include/Base/Entity.h>
#include <Include/Core.h>

namespace Borek {
namespace Panels {

class Scene {
public:
        Scene();
        void OnImguiRender();
        void SendOnSelectionChangeEvent();
        inline Entity GetSelectedEntity() { return m_SelectedEntity; }

private:
        Entity m_SelectedEntity;

        void DrawTreeNode(Entity e);
};

}  // namespace Panels
}  // namespace Borek
