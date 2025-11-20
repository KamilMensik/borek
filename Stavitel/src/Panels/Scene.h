// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/Event.h"
#include "Panels/PanelEvents.h"
#include <unordered_set>
#include <utility>
#include <vector>
#include <cstdint>

#include <Borek/Include/Base/Entity.h>
#include <Include/Core.h>

namespace Borek {
namespace Panels {

class Scene {
public:
        Scene();
        ~Scene();
        void OnImguiRender();
        void SendOnSelectionChangeEvent();
        inline Entity GetSelectedEntity() { return m_SelectedEntity; }
        void OnChangeEntity(ChangeEntityEvent& e);

private:
        Entity m_SelectedEntity;
        Entity m_Skip;
        std::string m_Search;

        enum class Operation : uint8_t {
                kOpen, kClose, kKeep,
        };
        std::vector<std::pair<Operation, Entity>> m_FlatGraph;
        std::unordered_set<uint32_t> m_VisibleTreeNodes;
        std::array<EVHandle, 1> m_EventHandles;

        void
        Phase1IterFuncBegin(Entity e);

        void
        Phase1IterFuncEnd(Entity e);

        void
        Phase2();

        void
        DrawTree();
};

}  // namespace Panels
}  // namespace Borek
