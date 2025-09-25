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
        void SetSelectedEntity(Entity e);

private:
        Entity m_SelectedEntity;
        Entity m_Skip;
        std::string m_Search;

        void
        EntityTreeNodeBegin(Entity e);
        void
        EntityTreeNodeEnd(Entity e);
};

}  // namespace Panels
}  // namespace Borek
