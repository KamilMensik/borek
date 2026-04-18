// Copyright 2024-2025 <kamilekmensik@gmail.com>

#pragma once

#include "Include/Events/Event.h"
#include "Panels/PanelEvents.h"
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
        bool OnChangeEntity(ChangeEntityEvent& e);

private:
        std::string m_Search;
        std::array<EVHandle, 1> m_EventHandles;
        Entity m_SelectedEntity;
};

}  // namespace Panels
}  // namespace Borek
