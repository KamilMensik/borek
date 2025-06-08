// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Base/Entity.h"
#include "Include/Base/Query.h"
#include <Include/Base/Scene.h>
#include <Include/Core.h>

namespace Borek {
namespace Panels {

class Scene {
public:
        Scene(Ref<Borek::Scene> scene);
        void OnImguiRender();
        void SendOnSelectionChangeEvent();
        inline Entity GetSelectedEntity() { return m_SelectedEntity; }

private:
        Ref<Borek::Scene> m_Scene;
        Entity m_SelectedEntity;
        Query m_EntityQuery;

        void DrawTreeNode(Entity e);
};

}  // namespace Panels
}  // namespace Borek
